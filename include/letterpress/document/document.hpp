#pragma once

#include <filesystem>
#include <string>

namespace lp::doc {

	/**
	 * @brief The Document class creates a drive agnostic (i.e., independent of any particular output format like PDF)
	 * representation of a document from a stream of instructions (write character, write whitespace, write paragraph,
	 * ...).
	 * @details In the overall context of Letterpress, the Document class is tasked with translating the parsed and
	 * executed tokenstream into the document's structure (how many pages, what content on which page, where to perform
	 * line- and paragraph-breaks, ...).
	 **/
	class Document final {
		/** \todo Maybe rename this class to DocumentBuilder to make clear that it constructs a document **/
	private:

	public:
		void writeText(std::string text);
		void writeWhiteSpace();
		void writeParagraph();
	};
}