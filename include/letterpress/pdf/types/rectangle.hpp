#pragma once

#include <string>

namespace lp::pdf {
	class Rectangle {
	private:
		float lowerleftx;
		float lowerlefty;
		float upperrightx;
		float upperrighty;

	public:
		Rectangle(float llx, float lly, float urx, float ury)
				: lowerleftx(llx), lowerlefty(lly), upperrightx(urx), upperrighty(ury) {}
	};
} // namespace lp::pdf