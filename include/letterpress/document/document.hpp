#pragma once

#include "../logging.hpp"
#include "../pdf/utils/fontfile.hpp"

#include <any>
#include <filesystem>
#include <string>
#include <variant>
#include <vector>

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

		enum class Mode {
			UnrestHorizMode,	/**< Adds material to the current horizontal list. **/
			/**
			 * Adds material to the current horizontal list. Restricted horizontal mode can not switch to vertical mode.
			 **/
			RestHorizMode,
			VertMode,			/**< Adds material to the current vertical list. **/
			InternVertMode,		/**< Adds material to the current vertical list. **/
			MathMode,
			DispMathMode
		};
	public:
		struct Glue {
			float amount;
		};
		using HElem = std::variant<
			char32_t,	/** < Character codepoint **/
			Glue
		>;
		/*class HBox;
		class VBox;
		using VElem = std::variant<
			HBox, VBox, Glue
		>;*/
		using HorizList = std::vector<HElem>;
		using VertList = std::vector<std::any>;
	private:
		lp::log::LoggerPtr logger;
		Mode mode = Mode::VertMode; /** \todo: is this the correct initial state? **/
		
		// std::vector<Page> pages;
		// lp::pdf::utils::FontFile currentfont;
	public: /** For debugging **/
		HorizList horizList;	
		VertList vertList;
	private:

		void shipHorizList();
	public:
		Document() noexcept;

		void addCharacter(char32_t character);
		void addWhitespace();
		void writeParagraph();
	};
}