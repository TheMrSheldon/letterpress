#include <letterpress/document/document.hpp>

#include <letterpress/document/boxes.hpp>
#include <letterpress/driver.hpp>
#include <letterpress/utils/overloaded.hpp>

#include <iostream>

using namespace lp::doc;
using namespace lp::utils;

Document::Document(lp::Driver& driver) noexcept : logger(lp::log::getLogger("DocProcessor")), driver(driver) {
	/** \todo Non-hardcoded width and height **/
	boxes.emplace(
			VBox{{.width = 210_mm, .height = 297_mm, .depth = 0_pt},
				 /* .content = */ {}}
	);
}

void Document::flush() {
	if (boxes.empty())
		return;
	while (boxes.size() > 1) {
		auto box = boxes.top();
		boxes.pop();
		auto& top = boxes.top();
		std::visit(
				overloaded{
						[this, &top](HBox& box) { ship(top, box); },
						[&top](VBox& box) { std::visit([&box](auto& top) { top.content.push_back(box); }, top); }
				},
				box
		);
	}
	auto& vbox = std::get<VBox>(boxes.top());
	driver.shipout(vbox);
	boxes.pop();
}

void Document::ship(AnyBox& top, HBox& hbox) const {
	logger->trace("Shipping horizontal list");
	std::visit(
			[&hbox](auto& top) {
				auto lines = lp::doc::linebreaking(hbox);
				for (auto&& box : lines)
					top.content.emplace_back(box);
			},
			top
	);
	if (boxes.size() == 1) {
		const VBox& top = std::get<VBox>(boxes.top());
		Dimension height = 0_pt;
		for (auto content : top.content) {
			std::visit(
					lp::utils::overloaded{
							[&height](const Box& box) { height += box.height; },
							[&height](const Glue& glue) { height += glue.idealwidth; },
							[](const Penalty& penalty) { /* ignore */ }
					},
					content
			);
		}
	}
}

void Document::pushFont(std::string path) noexcept {
	static const std::filesystem::path fontdirs[] = {"/usr/share/fonts", "/usr/local/share/fonts"};
	for (const auto& dir : fontdirs) {
		for (auto file : std::filesystem::recursive_directory_iterator(dir)) {
			if (!file.is_regular_file() || file.path().stem() != path)
				continue;
			auto font = std::make_shared<lp::pdf::utils::FontFile>(file.path());
			fonts.push(font);
			return;
		}
	}
	logger->error("Could not find font {}", path);
}

void Document::popFont() noexcept { fonts.pop(); }

lp::pdf::utils::FontFile& Document::currentFont() noexcept { return *fonts.top(); }
const lp::pdf::utils::FontFile& Document::currentFont() const noexcept { return *fonts.top(); }

void Document::addCharacter(char32_t character) {
	if (mode == Mode::VertMode || mode == Mode::InternVertMode) {
		logger->trace("Vert -> Horiz");
		// Switch to horizontal mode and start building a new paragraph (horizontal list)
		mode = Mode::UnrestHorizMode;
		auto& vbox = std::get<VBox>(boxes.top());
		boxes.push(HBox{{.width = vbox.width}});
	}
	/** \todo get width, height, and depth **/
	auto& font = fonts.top();
	assert(font.get() != nullptr);
	auto glyphIdx = font->getGlyphForChar(character);
	Glyph glyph{
			{.width = (font->getGlyphInfo(glyphIdx).advanceX).resolve(12, 0), /** \todo  use actual font size**/
			 .height = (font->getGlyphInfo(glyphIdx).height).resolve(12, 0)},
			/*.charcode =*/character,
			/*.font =*/font
	};
	auto& hbox = std::get<HBox>(boxes.top());

	/** Apply kerning if the last element is also a glyph and kerning should be added **/
	if (!hbox.content.empty()) {
		auto& prev = hbox.content.back();
		if (auto* pglyph = std::get_if<Glyph>(&prev)) {
			auto kerning = font->getKerning(pglyph->charcode, character);
			if (kerning != 0_pt) // Should this instead compare with some epsilon?
				hbox.content.push_back(Kerning{{.width = kerning.resolve(12, 0)}});
		}
	}

	hbox.content.push_back(glyph);
	logger->trace("{}", (char)character);
}

void Document::addWhitespace() {
	if (mode == Mode::RestHorizMode || mode == Mode::UnrestHorizMode) {
		addGlue({.idealwidth = (1_em / 3), .stretchability = (1_em / 6), .shrinkability = (1_em / 9)});
	}
}

void Document::addGlue(lp::doc::Glue glue) {
	/** \todo Resolve EM with actual font size **/
	auto resolved =
			Glue(glue.idealwidth.resolve(12, 0), glue.stretchability.resolve(12, 0), glue.shrinkability.resolve(12, 0));
	std::visit([resolved](auto& box) { box.content.emplace_back(resolved); }, boxes.top());
}

void Document::writeParagraph() {
	if (mode == Mode::RestHorizMode) {
		/** Do nothing in restricted horizontal mode. **/
	} else if (mode == Mode::UnrestHorizMode) {
		logger->trace("Horiz -> Vert");
		// Switch to vertical mode and ship out current paragraph
		mode = Mode::VertMode;
		auto hbox = std::get<HBox>(boxes.top());
		boxes.pop();
		/** \todo add infinite stretch where fitting **/
		hbox << Glue{.idealwidth = 0_pt, .stretchability = 1000000_pt, .shrinkability = 0_pt};
		ship(boxes.top(), hbox);
	} else {
		/** Do nothing. **/
	}
}