#pragma once

#include "../pdf/utils/fontfile.hpp"
#include "dimension.hpp"

#include <memory>
#include <variant>
#include <vector>

namespace lp::doc {

	struct Box {
		Dimension width;
		Dimension height;
		Dimension depth;
	};

	struct Glyph final : public Box {
		char32_t charcode;
		std::shared_ptr<lp::pdf::utils::FontFile> font;
	};

	struct Kerning final : public Box {};

	/** \cite knuthBreakingParagraphsLines1981 **/
	struct Glue final {
		/** Typically: 1/3 em **/
		Dimension idealwidth;
		/** Typically: 1/6 em **/
		Dimension stretchability;
		/** Typically: 1/9 em **/
		Dimension shrinkability;
	};

	struct Penalty final {
		int cost;		 /**< The cost of ending a line here. May be negative to encourage breaks. **/
		Dimension width; /**< The additional width if a break occurs at this point (e.g., due to a hyphen).**/
		bool flag;		 /**< The algorithm tries not to end consecutive rows on flagged penalties. **/
	};

	class VBox;
	class HBox;
	using AnyBox = std::variant<VBox, HBox>;

	struct HBox final : public Box {
		/** \todo support "disc" elements for discretionary breaks **/
		using Elem = std::variant<HBox, VBox, Glyph, Glue, Kerning, Penalty>;

		std::vector<Elem> content;

		template <typename T>
		HBox& operator<<(const T& item) {
			content.emplace_back(item);
			return *this;
		}
	};

	struct VBox final : public Box {
		using Elem = std::variant<HBox, VBox, Glue, Penalty>;

		std::vector<Elem> content;

		template <typename T>
		VBox& operator<<(const T& item) {
			content.emplace_back(item);
			return *this;
		}
	};

	std::vector<HBox> linebreaking(const HBox& hbox);

}; // namespace lp::doc
