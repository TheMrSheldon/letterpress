#pragma once

#include "../types/identifier.hpp"
#include "../font.hpp"

namespace lp::pdf {
	class Page;
}

namespace lp::pdf::utils {
	class Resources final {
		friend class lp::pdf::Page;
	private:
		QPDFObjectHandle handle;
		QPDFObjectHandle fonts;

		QPDFObjectHandle& getHandle();

		// void lp::pdf::Identifier add();
	public:
		Resources();

		lp::pdf::Identifier addFont(Font& font);
	};
}