#include <letterpress/pdf/utils/fontfile.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

using namespace lp::pdf::utils;

static FT_Library library;
static bool freetypeInitialized = false;
static void initFreetype() {
	auto logger = lp::log::getLogger("Font");
	if (!freetypeInitialized) {
		freetypeInitialized = true;
		auto error = FT_Init_FreeType(&library);
		if (error) {
			logger->critical("Failed to init FreeType: {}", FT_Error_String(error));
			abort();
		}
	}
}

FontFile::FontFile() : logger(lp::log::getLogger("Font")) {}

FontFile::FontFile(FontFile&& other)
		: logger(lp::log::getLogger("Font")), path(std::move(other.path)), face(std::move(other.face)),
		  hasKerningInfo(other.hasKerningInfo) {
	other.face = nullptr;
	other.path = "";
}

FontFile::FontFile(std::filesystem::path path, std::filesystem::path afmPath)
		: logger(lp::log::getLogger("Font")), path(path) {
	logger->trace("Loading {}", path.c_str());
	initFreetype();
	auto error = FT_New_Face(library, path.c_str(), 0, &face);
	if (error) {
		auto msg = FT_Error_String(error);
		logger->error("Error opening font file: {} -- {}", error, (msg ? msg : "<no message>"));
		abort(); /** \todo more graceful **/
	}
	if (!afmPath.empty()) {
		error = FT_Attach_File(face, afmPath.c_str());
		if (error) {
			auto msg = FT_Error_String(error);
			logger->error("Error opening font attachment: {} -- {}", error, (msg ? msg : "<no message>"));
			abort(); /** \todo more graceful **/
		}
	}
	hasKerningInfo = FT_HAS_KERNING(face);
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	FT_Set_Char_Size(face, 0, 1, 0, 0);
	logger->trace(
			"\tKerning Info  : {} \tScalable : {}", (hasKerningInfo ? "yes" : "no"),
			(FT_IS_SCALABLE(face) ? "yes" : "no")
	);
	logger->trace("\tFont Height   : {} \tEM Size  : {}", face->size->metrics.height, face->units_per_EM);
	logger->trace(
			"\tPixels Per EM : {}\u00D7{} \tScale    : {}\u00D7{}", face->size->metrics.x_ppem,
			face->size->metrics.y_ppem, face->size->metrics.x_scale, face->size->metrics.y_scale
	);
	std::string charmapinfo = "";
	for (int i = 0; i < face->num_charmaps; i++) {
		// Note: could be pretty-printed using information from #include <freetype/ttnameid.h>
		auto charmap = face->charmaps[i];
		charmapinfo +=
				(charmapinfo.empty() ? "" : ", ") + std::format("{}>{}", charmap->encoding_id, charmap->platform_id);
	}
	logger->trace("\tCharmaps: {}", charmapinfo);
}

FontFile::~FontFile() { destroy(); }

FontFile& FontFile::operator=(FontFile&& other) {
	destroy();
	face = std::move(other.face);
	other.face = nullptr;
	path = other.path;
	other.path = "";
	hasKerningInfo = other.hasKerningInfo;
	return *this;
}

void FontFile::destroy() noexcept {
	if (face == nullptr)
		return;
	FT_Done_Face(face);
	face = nullptr;
	path = "";
}

std::filesystem::path FontFile::getPath() const noexcept { return path; }

std::string FontFile::getFamilyName() const noexcept { return face->family_name; }

short FontFile::getAscent() const noexcept { return face->ascender; }
short FontFile::getDescent() const noexcept { return face->descender; }
void FontFile::getBoundingBox(double& xmin, double& ymin, double& xmax, double& ymax) const noexcept {
	xmin = face->bbox.xMin;
	xmax = face->bbox.xMax;
	ymin = face->bbox.yMin;
	ymax = face->bbox.yMax;
}

unsigned FontFile::getGlyphForChar(char32_t c) const noexcept { return FT_Get_Char_Index(face, c); }

FontFile::GlyphInfo FontFile::getGlyphInfo(unsigned glyph) const noexcept {
	auto error = FT_Load_Glyph(face, glyph, FT_LOAD_NO_BITMAP | FT_LOAD_NO_SCALE);
	if (error) {
		auto msg = FT_Error_String(error);
		logger->critical(
				"[{}] Error loading glyph: {} -- {}", path.filename().string(), error, (msg ? msg : "<no message>")
		);
		abort(); /** \todo more graceful **/
	}
	/*float scale = 1.0f;
	if (!FT_IS_SCALABLE(face)) {
		scale = 1.0f / (float)face->size->metrics.x_scale;
	}*/
	/** \todo why does this seem to work? */
	auto scale = 1.0f / 1000.0f *
				 (face->size->metrics.x_scale / 4194.0f
				 ); /// (float)face->size->metrics.x_scale; //1.0f/64.0f * 1.0f/ (float)face->size->metrics.x_scale;
	return {
			.advanceX = face->glyph->advance.x * scale,
			.width = face->glyph->metrics.width,
			.height = face->glyph->metrics.height,
	};
}

// https://freetype.org/freetype2/docs/tutorial/step2.html
float FontFile::getKerning(char32_t left, char32_t right) const noexcept {
	if (!hasKerningInfo) {
		return 0;
	}
	auto leftGlyph = getGlyphForChar(left);
	auto rightGlyph = getGlyphForChar(right);
	FT_Vector kerning;
	auto error = FT_Get_Kerning(face, leftGlyph, rightGlyph, FT_KERNING_DEFAULT, &kerning);
	if (error) {
		auto msg = FT_Error_String(error);
		logger->critical(
				"[{}] Error fetching kerning: {} -- {}", path.filename().string(), error, (msg ? msg : "<no message>")
		);
	}
	return kerning.x / 64.0f;
}