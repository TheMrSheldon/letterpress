#pragma once

namespace lp::doc {
	class ILayout {
	private:
	public:
		virtual void shipout() = 0;
	};
} // namespace lp::doc