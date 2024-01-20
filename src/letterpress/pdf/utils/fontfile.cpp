#include <letterpress/pdf/utils/fontfile.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

using namespace lp::pdf::utils;

static FT_Library library;
static bool freetypeInitialized = false;
static void initFreetype() {
	if (!freetypeInitialized) {
		freetypeInitialized = true;
		auto error = FT_Init_FreeType(&library);
		if (error) {
			std::cout << FT_Error_String(error) << std::endl;
		}
	}
}

FontFile::FontFile(std::string path, std::string afmPath) {
	initFreetype();
	auto error = FT_New_Face(library, path.c_str(), 0, &face);
	if (error) {
		std::cout << "Error opening font file: " << error << std::endl;
		auto msg = FT_Error_String(error);
		std::cout << (msg? msg:"--") << std::endl;
	}
	if (!afmPath.empty()) {
		error = FT_Attach_File(face, afmPath.c_str());
		if (error) {
			std::cout << "Error opening font attachment: " << error << std::endl;
			auto msg = FT_Error_String(error);
			std::cout << (msg? msg:"--") << std::endl;
		}
	}
	hasKerningInfo = FT_HAS_KERNING(face);
	std::cout << "Kerning Info: " << (hasKerningInfo?'t':'f') << std::endl;
	std::cout << "Scalable: " << (FT_IS_SCALABLE(face)?'t':'f') << std::endl;
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	FT_Set_Char_Size(face, 0, 1, 0, 0);
	std::cout << "Height: " << face->size->metrics.height << std::endl;
	std::cout << "X Scale: " << face->size->metrics.x_scale << std::endl;
	std::cout << "X Pixels Per EM: " << face->size->metrics.x_ppem << std::endl;
	for (int i = 0; i < face->num_charmaps; i++) {
		auto charmap = face->charmaps[i];
		std::cout << '[' << i << ']' << charmap->encoding_id << ", " << charmap->platform_id << std::endl;
		//if (charmap->encoding_id == 1 && charmap->platform_id == 3) {
		//	FT_Set_Charmap(face, charmap);
		//}
	}
}

std::string FontFile::getFamilyName() const noexcept {
	return face->family_name;
}

short FontFile::getAscent() const noexcept { return face->ascender; }
short FontFile::getDescent() const noexcept { return face->descender; }
void FontFile::getBoundingBox(double& xmin, double& ymin, double& xmax, double& ymax) const noexcept {
	xmin = face->bbox.xMin;
	xmax = face->bbox.xMax;
	ymin = face->bbox.yMin;
	ymax = face->bbox.yMax;
}

unsigned FontFile::getGlyphForChar(char32_t c) const noexcept {
	return FT_Get_Char_Index(face, c);
}

FontFile::GlyphInfo FontFile::getGlyphInfo(unsigned glyph) const noexcept {
	auto error = FT_Load_Glyph(face, glyph, FT_LOAD_NO_BITMAP | FT_LOAD_NO_SCALE);
	if (error) {
		std::cout << FT_Error_String(error) << std::endl;
	}
	/*float scale = 1.0f;
	if (!FT_IS_SCALABLE(face)) {
		scale = 1.0f / (float)face->size->metrics.x_scale;
	}*/
	/** TODO: why does this seem to work? */
	auto scale = 1.0f/1000.0f * (face->size->metrics.x_scale / 4194.0f);/// (float)face->size->metrics.x_scale; //1.0f/64.0f * 1.0f/ (float)face->size->metrics.x_scale;
	return {
		.advanceX = face->glyph->advance.x * scale,
		.width = face->glyph->metrics.width,
		.height = face->glyph->metrics.width,
	};
}

// https://freetype.org/freetype2/docs/tutorial/step2.html
float FontFile::getKerning(char left, char right) const noexcept {
	if (!hasKerningInfo) {
		return 0;
	}
	auto leftGlyph = getGlyphForChar(left);
	auto rightGlyph = getGlyphForChar(right);
	FT_Vector kerning;
	auto error = FT_Get_Kerning(face, leftGlyph, rightGlyph, FT_KERNING_DEFAULT, &kerning);
	if (error) {
		//std::cout << FT_Error_String(error) << std::endl;
		std::cout << "Error: " << error << std::endl;
	}
	return kerning.x / 64.0f;
}