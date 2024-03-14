#pragma once

#include "../pdf/utils/fontfile.hpp"

#include <memory>
#include <variant>
#include <vector>

namespace lp::doc {

	struct Box {
		float width;
		float height;
		float depth;
	};

	struct Glyph : public Box {
		char32_t charcode;
		std::shared_ptr<lp::pdf::utils::FontFile> font;
	};

	struct Kerning : public Box {};

	/** http://www.eprg.org/G53DOC/pdfs/knuth-plass-breaking.pdf **/
	struct Glue {
		/** Typically: 1/3 em **/
		float idealwidth;
		/** Typically: 1/6 em **/
		float stretchability;
		/** Typically: 1/9 em **/
		float shrinkability;
	};

	struct Penalty {
		int cost; /**< The cost of ending a line here. May be negative to encourage breaks. **/
		float width; /**< The additional width if a break occurs at this point (e.g., due to a hyphen).**/
		bool flag; /**< The algorithm tries not to end consecutive rows on flagged penalties. **/
	};

	class VBox;
	class HBox;
	using AnyBox = std::variant<VBox, HBox>;

	struct HBox : public Box {
		using Elem = std::variant<
			HBox,
			VBox,
			Glyph,
			Glue,
			Kerning
		>;

		std::vector<Elem> content;
	};

	struct VBox : public Box {
		using Elem = std::variant<
			HBox,
			VBox,
			Glue
		>;

		std::vector<Elem> content;
	};

};
