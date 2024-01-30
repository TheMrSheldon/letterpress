#include <letterpress/document/document.hpp>

#include <iostream>

using namespace lp::doc;

Document::Document() noexcept : logger(lp::log::getLogger("DocProcessor")) {}

void Document::shipHorizList() {
    logger->trace("Shipping horizontal list");
    /** \todo break paragraphs **/
    vertList.push_back(std::move(horizList));
}

void Document::addCharacter(char32_t character) {
    if (mode == Mode::VertMode || mode == Mode::InternVertMode) {
        logger->trace("Vert -> Horiz");
        // Switch to horizontal mode and start building a new paragraph (horizontal list)
        mode = Mode::UnrestHorizMode;
        horizList.clear();
    }
    /** \todo check font for kerning **/
    horizList.emplace_back(character);
}
void Document::addWhitespace() {
    if (mode == Mode::RestHorizMode || mode == Mode::UnrestHorizMode) {
        /** TODO: insert correct glue **/
        horizList.emplace_back(Glue{10.0f});
    }
}
void Document::writeParagraph() {
    if (mode == Mode::RestHorizMode) {
        /** Do nothing in restricted horizontal mode. **/
    } else if (mode == Mode::UnrestHorizMode) {
        logger->trace("Horiz -> Vert");
        // Switch to vertical mode and ship out current paragraph
        mode = Mode::VertMode;
        shipHorizList();
    } else {
        /** Do nothing. **/
    }
}