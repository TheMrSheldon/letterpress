#pragma once

#include <qpdf/QPDF.hh>

#include <filesystem>
#include <memory>
#include <vector>

namespace lp::pdf {

	class Font;

	class Page;

	class PDF final {
		friend Font;
		friend Page;
	private:
		QPDF handle;
		std::vector<std::unique_ptr<Page>> pages;
		// std::vector<std::reference_wrapper<Font>> fontsToSubset;
		std::vector<std::unique_ptr<Font>> fonts;

		QPDF& getHandle();
		void subsetFonts();
	public:
		PDF();

		Page& addPage();
		Font& addFont(std::filesystem::path path, std::filesystem::path afmPath = "");

		void save(std::filesystem::path path);
	};

}