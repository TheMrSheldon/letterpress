#pragma once

#include <qpdf/QPDF.hh>

#include <filesystem>
#include <memory>
#include <vector>

namespace lp::pdf::utils {
	class FontFile;
	using FontFilePtr = std::shared_ptr<FontFile>;
}

namespace lp::pdf {

	class Font;

	class Page;

	class PDF final {
		friend Font;
		friend Page;

	private:
		QPDF handle;
		std::vector<std::unique_ptr<Page>> pages;
		std::map<utils::FontFilePtr, std::unique_ptr<Font>> fonts;

		QPDF& getHandle();
		void subsetFonts();

	public:
		PDF();

		Page& addPage();

		/**
		 * @brief Registers the font file as a font associated with this PDF document.
		 * @details If the same font file was previously added, no new Font object is created.
		 * @param font The font to be added.
		 * @return A lp::pdf::Font object wrapping the font file.
		 */
		Font& registerFont(utils::FontFilePtr font);

		void save(std::filesystem::path path);
	};

} // namespace lp::pdf