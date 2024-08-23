#ifndef LP_DRIVER_HPP
#define LP_DRIVER_HPP

#include "document/boxes.hpp"
#include "document/document.hpp"

namespace lp {
	class Driver {
	private:
	public:
		virtual ~Driver() = default;

		virtual void shipout(const lp::doc::VBox& page) = 0;
	};
} // namespace lp
#endif