#ifndef LP_DRIVER_HPP
#define LP_DRIVER_HPP

#include "document/boxes.hpp"

#include <optional>
#include <string>
#include <vector>

namespace lp {
	class Driver {
	private:
	public:
		virtual ~Driver() = default;

		virtual void shipout(const lp::doc::VBox& page) = 0;

		/** Metadata **/
		virtual void setAuthor(std::optional<std::string> author) = 0;
		virtual void setDate(std::optional<std::string> date) = 0;
		virtual void setTitle(std::optional<std::string> title) = 0;
		virtual void setDescription(std::optional<std::string> description) = 0;
		virtual void setKeywords(std::vector<std::string> keywords) = 0;
	};
} // namespace lp
#endif