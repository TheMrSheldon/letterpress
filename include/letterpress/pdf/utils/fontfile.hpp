#pragma once

#include "../../document/dimension.hpp"
#include "../../logging.hpp"

#include <filesystem>
#include <inttypes.h>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace lp::pdf::utils {

	class FontFile final {
	private:
		lp::log::LoggerPtr logger;
		struct GlyphInfo {
			lp::doc::Dimension advanceX;
			lp::doc::Dimension width, height;
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
		explicit FontFile(std::filesystem::path path, std::filesystem::path afmPath = "");
		~FontFile();
		FontFile& operator=(FontFile&& other);

		std::filesystem::path getPath() const noexcept;

		std::string getFamilyName() const noexcept;

		short getAscent() const noexcept;
		short getDescent() const noexcept;
		float getXEm() const noexcept;
		void getBoundingBox(double& xmin, double& ymin, double& xmax, double& ymax) const noexcept;
		unsigned getGlyphForChar(char32_t c) const noexcept;
		GlyphInfo getGlyphInfo(unsigned glyph) const noexcept;

		bool containsChar(char32_t c) const noexcept;
		lp::doc::Dimension getKerning(char32_t left, char32_t right) const noexcept;
	};

} // namespace lp::pdf::utils