#pragma once

#include <inttypes.h>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace lp::pdf::utils {

	class FontFile final {
	private:
		struct GlyphInfo {
			float advanceX;
			float width, height;
		};
		
		FT_Face face;
		bool hasKerningInfo;

	public:
		FontFile(std::string path, std::string afmPath = "");

		std::string getFamilyName() const noexcept;

		short getAscent() const noexcept;
		short getDescent() const noexcept;
		void getBoundingBox(double& xmin, double& ymin, double& xmax, double& ymax) const noexcept;
		unsigned getGlyphForChar(char32_t c) const noexcept;
		GlyphInfo getGlyphInfo(unsigned glyph) const noexcept;

		bool containsChar(char c) const noexcept;
		float getKerning(char left, char right) const noexcept;
	};

}