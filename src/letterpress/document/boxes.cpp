#include <letterpress/document/boxes.hpp>

#include <letterpress/utils/overloaded.hpp>

using lp::doc::HBox;
using lp::utils::overloaded;

static constexpr float InfBadness = 100000;

namespace lp::doc {
	class Justifier final {
	private:
		static constexpr float InfBadness = 100'000;
		static constexpr int FlagPenalty = 3000;
		struct Break {
			size_t demerit;
			Break* previousBreak;
			Dimension shrinkability;
			Dimension width;
			Dimension stretchability;
		};

		float
		demerit(const Dimension lineWidth, const Dimension& width, const Dimension& stretchability,
				const Dimension& shrinkability, int penalty, bool flagPenalty) {
			const auto& denominator = (width < lineWidth) ? stretchability : shrinkability;
			auto adjustmentRatio = (lineWidth - width).getPoint() / denominator.getPoint();
			auto badness = (adjustmentRatio < -1) ? InfBadness : (100 * std::pow(std::abs(adjustmentRatio), 3));
			auto flagPenaltyValue = flagPenalty ? FlagPenalty : 0;
			if (penalty >= 0)
				return std::pow(1 + badness + penalty, 2) + flagPenaltyValue;
			else if (InfBadness < penalty)
				return std::pow(1 + badness, 2) - penalty * penalty + flagPenaltyValue;
			else
				return std::pow(1 + badness, 2) + flagPenaltyValue;
		}

		HBox constructBox(const std::forward_iterator auto& begin, const std::forward_iterator auto& end) {
			HBox box{{.width = 0_cm, .height = 0_cm, .depth = 0_cm}};
			// Skip first spaces and penalties
			auto it = begin;
			while (it != end && (std::holds_alternative<Glue>(*it) || std::holds_alternative<Penalty>(*it)))
				++it;
			// Add glue and boxes to box
			for (; it != end; ++it) {
				std::visit(
						overloaded{
								[&box](const Glue& glue) {
									box << glue;
									/** before constructBox, glue should be scaled and its true width stored in
									 * idealwidth */
									box.width += glue.idealwidth;
								},
								[&box](const is_box auto& b) {
									box << b;
									box.width += b.width;
									box.height = std::max(box.height, b.height);
									box.depth = std::max(box.depth, b.depth);
								},
								[](const auto&) { /** ignore **/ }
						},
						*it
				);
			}
			return box;
		}

		template <typename TElem>
		static void adjustGlue(TElem* begin, TElem* end, const Break& breakpoint, const Dimension& lineWidth) {
			const auto& denominator =
					(breakpoint.width < lineWidth) ? breakpoint.stretchability : breakpoint.shrinkability;
			auto adjustmentRatio = (lineWidth - breakpoint.width).getPoint() / denominator.getPoint();
			for (auto it = begin; it != end; ++it) {
				std::visit(
						overloaded{
								[adjustmentRatio](Glue& glue) {
									glue.idealwidth +=
											((adjustmentRatio >= 0) ? glue.stretchability : glue.shrinkability) *
											adjustmentRatio;
								},
								[](const auto&) { /* ignore */ }
						},
						*it
				);
			}
		}

	public:
		template <typename TBox, typename TElem>
		std::generator<TBox>
		linebreaking(const std::forward_iterator auto begin, const std::forward_iterator auto end) {
			/** \todo break paragraphs non-greedily **/
			/** \todo remove hardcoded width **/
			std::vector<TElem> buffer;
			std::vector<Break> candidates;
			candidates.emplace_back(Break{
					.demerit = 0, .previousBreak = 0, .shrinkability = 0_pt, .width = 0_pt, .stretchability = 0_pt
			});
			for (auto it = begin; it != end; ++it) {
				Break& cur = candidates.back();
				TElem& element = buffer.emplace_back(*it);
				std::visit(
						overloaded{
								[&cur](const Box& box) { cur.width += box.width; },
								[&cur](const Glue& glue) {
									cur.width += glue.idealwidth;
									cur.shrinkability += glue.shrinkability;
									cur.stretchability += glue.stretchability;
								},
								[](const Penalty& penalty) { /* do nothing */ }
						},
						element
				);
				if (cur.width + cur.stretchability >= 210_mm - 4_cm && std::holds_alternative<Glue>(element)) {
					adjustGlue(&*buffer.begin(), &*buffer.end(), cur, 210_mm - 4_cm);
					co_yield constructBox(buffer.begin(), buffer.end());
					buffer.clear();
					candidates.clear();
					candidates.emplace_back(
							Break{.demerit = 0,
								  .previousBreak = 0,
								  .shrinkability = 0_pt,
								  .width = 0_pt,
								  .stretchability = 0_pt}
					);
				}
			}
			if (!buffer.empty()) {
				Break& cur = candidates.back();
				adjustGlue(&*buffer.begin(), &*buffer.end(), cur, 210_mm - 4_cm);
				co_yield constructBox(buffer.begin(), buffer.end());
			}
		}
	};
} // namespace lp::doc

std::generator<HBox> lp::doc::linebreaking(const HBox& hbox) {
	Justifier justifier;
	return justifier.linebreaking<HBox, HBox::Elem>(hbox.content.begin(), hbox.content.end());
}