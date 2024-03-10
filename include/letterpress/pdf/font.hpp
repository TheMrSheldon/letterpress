#pragma once

#include "font_descriptor.hpp"

#include <qpdf/QPDFObjectHandle.hh>

#include <memory>
#include <string>
#include <vector>

namespace lp::pdf::utils {
	class Resources;

	class FontFile;
	using FontFilePtr = std::shared_ptr<FontFile>;
}

namespace lp::pdf {
	class PDF;

	class Font final {
		friend lp::pdf::utils::Resources;
		friend PDF;
	private:
		PDF& pdf;
		utils::FontFilePtr file;
		QPDFObjectHandle handle;
		FontDescriptor descriptor;
		std::vector<unsigned> subsetGlyphs;
		std::shared_ptr<QPDFObjectHandle::StreamDataProvider> data;

		QPDFObjectHandle& getHandle();
	public:
		/** 
		 * @brief Creates a new font object. Do not call this directly, use PDF::addFont instead!
		 **/
		Font(PDF& pdf, utils::FontFilePtr file);

		unsigned getGlyphForChar(char c) const;
		float getKerning(unsigned leftGlyph, unsigned rightGlyph) const;

		void addToSubset(unsigned glyph);

		void computeSubset();
	};

	using FontPtr = std::shared_ptr<Font>;

}