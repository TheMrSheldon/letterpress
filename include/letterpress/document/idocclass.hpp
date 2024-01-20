#pragma once

#include "ilayout.hpp"

namespace lp::doc {
	class IDocClass {
	private:

	public:
		virtual ILayout& getLayout() = 0;
	};
}