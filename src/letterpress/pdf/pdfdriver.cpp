#include <letterpress/pdf/pdfdriver.hpp>

#include <letterpress/pdf/page.hpp>
#include <letterpress/utils/overloaded.hpp>

using namespace lp;
using namespace lp::doc;
using namespace lp::pdf;

PDFDriver::PDFDriver(std::filesystem::path outfile) : outfile(outfile), pdf() {}

PDFDriver::~PDFDriver() { pdf.save(outfile); }

inline std::string cpp20_codepoint_to_utf8(char32_t cp) // C++20 Sandard
{
	using codecvt_32_8_type = std::codecvt<char32_t, char8_t, std::mbstate_t>;

	struct codecvt_utf8 : public codecvt_32_8_type {
		codecvt_utf8(std::size_t refs = 0) : codecvt_32_8_type(refs) {}
	};

	char8_t utf8[4];
	char8_t* end_of_utf8;

	char32_t const* from = &cp;

	std::mbstate_t mbs;
	codecvt_utf8 ccv;

	if (ccv.out(mbs, from, from + 1, from, std::begin(utf8), std::end(utf8), end_of_utf8))
		throw std::runtime_error("bad conversion");

	return {reinterpret_cast<char*>(utf8), reinterpret_cast<char*>(end_of_utf8)};
}

void PDFDriver::writeVBox(lp::pdf::Page& page, const lp::doc::VBox& vbox) {
	for (auto&& elem : vbox.content) {
		std::visit(
				lp::utils::overloaded{
						[this, &page](const lp::doc::VBox& vbox) { writeVBox(page, vbox); },
						[this, &page](const lp::doc::HBox& hbox) { writeHBox(page, hbox); },
						[this, &page](const lp::doc::Glue& glue) { /** \todo implement **/ },
						[this, &page](const lp::doc::Penalty& penalty) { /** \todo implement **/ }
				},
				elem
		);
	}
}

void PDFDriver::writeHBox(lp::pdf::Page& page, const lp::doc::HBox& hbox) {
	auto& stream = page.getContentStream();
	lp::pdf::Array array;
	std::string text;
	Dimension width = 0_pt;
	/** \todo this code should be more readable**/
	for (auto&& he : hbox.content) {
		if (const auto val = std::get_if<lp::doc::Glyph>(&he)) {
			auto& font = pdf.registerFont(val->font);
			bool fontChange = stream.getGraphicsState().font != &font;
			if (fontChange) {
				if (!text.empty()) {
					/** Push text **/
					array.append(text);
					text = "";
					width = 0_pt;
				}
				stream.getStreamWriter().showTextAdjusted(array);
				array = {};
				/** Change font **/
				stream.setFont(font, 12);
			}
			text += cpp20_codepoint_to_utf8(val->charcode);
			width += val->width;
		} else if (const auto val = std::get_if<lp::doc::Glue>(&he)) {
			if (!text.empty()) {
				array.append(text);
				text = "";
				width = 0_pt;
			}
			array.append(-val->idealwidth.getPoint() * 83); /** \todo why 83? **/
		} else {											/** \todo add kerning support **/
			throw std::runtime_error("Unexpected datatype");
		}
	}
	if (!text.empty())
		array.append(text);
	/** \todo remove hardcoded 1.2*lineskip**/
	stream.getStreamWriter().showTextAdjusted(array).moveText(0, (-12) * 1.2);
}

void PDFDriver::createGraphic() {}

void PDFDriver::shipout(const lp::doc::VBox& page) {
	auto& pdfpage = pdf.addPage();
	int width = (int)pdfpage.toUserSpace(page.width);
	int height = (int)pdfpage.toUserSpace(page.height);
	pdfpage.setMediaBox(0, 0, width, height);

	auto& stream = pdfpage.getContentStream();
	stream.beginText().moveText((2_cm).getPoint(), height - (1_em + 2_cm).resolve(12).getPoint());
	writeVBox(pdfpage, page);
	stream.endText();
}

void PDFDriver::setAuthor(std::optional<std::string> author) { /** \todo implement **/ }
void PDFDriver::setDate(std::optional<std::string> date) { /** \todo implement **/ }
void PDFDriver::setTitle(std::optional<std::string> title) { /** \todo implement **/ }
void PDFDriver::setDescription(std::optional<std::string> description) { /** \todo implement **/ }
void PDFDriver::setKeywords(std::vector<std::string> keywords) { /** \todo implement **/ }