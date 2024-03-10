#include <letterpress/document/document.hpp>

#include <letterpress/document/boxes.hpp>
#include <letterpress/driver.hpp>

#include <iostream>

using namespace lp::doc;

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

Document::Document(lp::Driver& driver) noexcept : logger(lp::log::getLogger("DocProcessor")), driver(driver) {
    /** \todo Non-hardcoded width and height **/
    boxes.emplace(VBox{
        {
            .width = 210,
            .height = 297,
            .depth = 0
        },
        /* .content = */ {}
    });
}

void Document::flush() {
    if (boxes.empty())
        return;
    while (boxes.size() > 1) {
        auto box = boxes.top();
        boxes.pop();
        auto& top = boxes.top();
        std::visit(overloaded{
            [this, &top](HBox& box) {
                ship(top, box);
            },
            [&top](VBox& box) {
                std::visit([&box](auto& top) { top.content.push_back(box); }, top);
            }
        }, box);
    }
    auto& vbox = std::get<VBox>(boxes.top());
    driver.shipout(vbox);
    boxes.pop();
}

void Document::ship(AnyBox& top, HBox& hbox) const {
    logger->trace("Shipping horizontal list");
    /** \todo break paragraphs non-greedily **/
    /** \todo remove hardcoded width **/
    HBox current{{.width=36000, .height=hbox.height}};
    float width = 0;
    for (auto&& tmp : hbox.content) {
        /** Skip glue at the beginning **/
        if (!std::holds_alternative<Glue>(tmp) || !current.content.empty()) {
            current.content.push_back(tmp);
            std::visit(overloaded{
                [&width](auto& box) { width += box.width; },
                [&width](Glue& glue) { width += glue.idealwidth; }
            }, tmp);
        }
        if (width >= current.width && std::holds_alternative<Glue>(tmp)) {
            /** \todo space correctly instead of evenly **/
            auto count = std::count_if(std::begin(current.content), std::end(current.content), [](lp::doc::HBox::Elem& element) { return std::holds_alternative<Glue>(element); });
            float glueWidth = (39000 - width) / count;
            for (auto&& element : current.content)
                if (std::holds_alternative<Glue>(element))
                    std::get<Glue>(element).idealwidth += glueWidth;

            width = 0;
            std::visit([&current](auto& top) { top.content.push_back(current); }, top);
            current.content.clear();
        }
    }
    if (!current.content.empty())
        std::visit([&current](auto& top) { top.content.push_back(current); }, top);
}

void Document::pushFont(std::shared_ptr<lp::pdf::utils::FontFile> font) noexcept {
    fonts.push(font);
}

void Document::popFont() noexcept {
    fonts.pop();
}

void Document::addCharacter(char32_t character) {
    if (mode == Mode::VertMode || mode == Mode::InternVertMode) {
        logger->trace("Vert -> Horiz");
        // Switch to horizontal mode and start building a new paragraph (horizontal list)
        mode = Mode::UnrestHorizMode;
        auto& vbox = std::get<VBox>(boxes.top());
        boxes.push(HBox{{.width=vbox.width}});
    }
    /** \todo check font for kerning **/
    /** \todo get width, height, and depth **/
    auto& font = fonts.top();
    assert(font.get() != nullptr);
    auto glyphIdx = font->getGlyphForChar(character);
    Glyph glyph {
        {
            .width = font->getGlyphInfo(glyphIdx).advanceX*1000.0f,
            .height = font->getGlyphInfo(glyphIdx).height
        },
        /*.charcode =*/ character,
        /*.font =*/ font
    };
    auto& hbox = std::get<HBox>(boxes.top());
    hbox.content.push_back(glyph);
}
void Document::addWhitespace() {
    if (mode == Mode::RestHorizMode || mode == Mode::UnrestHorizMode) {
        /** TODO: insert correct glue **/
        auto& hbox = std::get<HBox>(boxes.top());
        hbox.content.push_back(Glue(300.0f));
    }
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
        ship(boxes.top(), hbox);
    } else {
        /** Do nothing. **/
    }
}