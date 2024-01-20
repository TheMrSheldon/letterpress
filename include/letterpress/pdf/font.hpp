#pragma once

#include <string>
#include <vector>

#include <letterpress/pdf/utils/fontfile.hpp>
#include "font_descriptor.hpp"

#include <qpdf/QPDFObjectHandle.hh>

namespace lp::pdf::utils {
	class Resources;
}

namespace lp::pdf {
	class PDF;

	class Font final {
		friend lp::pdf::utils::Resources;
	private:
		PDF& pdf;
		utils::FontFile file;
		QPDFObjectHandle handle;
		FontDescriptor descriptor;
		std::vector<unsigned> subsetGlyphs;
		std::shared_ptr<QPDFObjectHandle::StreamDataProvider> data;

		QPDFObjectHandle& getHandle();
	public:
		Font(PDF& pdf, std::string path, std::string afmPath = "");

		unsigned getGlyphForChar(char c) const;
		float getKerning(unsigned leftGlyph, unsigned rightGlyph) const;

		void addToSubset(unsigned glyph);

		void computeSubset();
	};

}