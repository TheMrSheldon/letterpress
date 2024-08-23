#include <letterpress/document/boxes.hpp>

#include <letterpress/utils/overloaded.hpp>

using lp::doc::HBox;
using lp::utils::overloaded;

std::vector<HBox> lp::doc::linebreaking(const HBox& hbox) {
	/** \todo break paragraphs non-greedily **/
	/** \todo remove hardcoded width **/
	std::vector<HBox> ret;
	HBox current{{.width = 39000, .height = hbox.height}};
	float width = 0;
	for (auto&& tmp : hbox.content) {
		/** Skip glue at the beginning **/
		if (!std::holds_alternative<Glue>(tmp) || !current.content.empty()) {
			current.content.push_back(tmp);
			std::visit(
					overloaded{
							[&width](const auto& box) { width += box.width; },
							[&width](const Glue& glue) { width += glue.idealwidth; }
					},
					tmp
			);
		}
		if (width >= current.width - 2000 && std::holds_alternative<Glue>(tmp)) {
			/** \todo space correctly instead of evenly **/
			auto count = std::count_if(
					std::begin(current.content), std::end(current.content),
					[](lp::doc::HBox::Elem& element) { return std::holds_alternative<Glue>(element); }
			);
			float glueWidth = (current.width - width) / count;
			for (auto&& element : current.content)
				if (std::holds_alternative<Glue>(element))
					std::get<Glue>(element).idealwidth += glueWidth;

			width = 0;
			ret.push_back(current);
			current.content.clear();
		}
	}
	if (!current.content.empty())
		ret.push_back(current);
	return ret;
}