#pragma once

#include <filesystem>
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

		std::filesystem::path path;
		
		FT_Face face;
		bool hasKerningInfo;

		FontFile(const FontFile& other) = delete;
		FontFile& operator=(const FontFile& other) = delete;

		void destroy() noexcept;
	public:
		FontFile();
		FontFile(FontFile&& other);
		FontFile(std::filesystem::path path, std::filesystem::path afmPath = "");
		~FontFile();
		FontFile& operator=(FontFile&& other);

		std::filesystem::path getPath() const noexcept;

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