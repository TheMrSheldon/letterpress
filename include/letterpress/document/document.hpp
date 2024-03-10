#pragma once

#include "boxes.hpp"
#include "../logging.hpp"
#include "../pdf/utils/fontfile.hpp"

#include <any>
#include <filesystem>
#include <memory>
#include <stack>
#include <string>
#include <variant>
#include <vector>

namespace lp {
	class Driver;
};

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

		lp::Driver& driver;

		lp::log::LoggerPtr logger;
		/** \todo maybe this can be removed and inferred from the "boxes" stack **/
		Mode mode = Mode::VertMode; /** \todo: is this the correct initial state? **/
		
		std::stack<AnyBox> boxes;

		std::stack<std::shared_ptr<lp::pdf::utils::FontFile>> fonts;

		void ship(AnyBox& top, HBox& hbox) const;
	public:
		Document(lp::Driver& driver) noexcept;

		void pushFont(std::shared_ptr<lp::pdf::utils::FontFile> font) noexcept;
		void popFont() noexcept;
		void addCharacter(char32_t character);
		void addWhitespace();
		void writeParagraph();

		void flush();
	};
}