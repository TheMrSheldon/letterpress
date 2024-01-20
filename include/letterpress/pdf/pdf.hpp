#pragma once

#include "page.hpp"

#include <qpdf/QPDF.hh>

#include <filesystem>
#include <memory>
#include <vector>

namespace lp::pdf {

	class PDF final {
		friend class Font;
		friend class Page;
	private:
		QPDF handle;
		std::vector<std::unique_ptr<Page>> pages;
		std::vector<std::reference_wrapper<Font>> fontsToSubset;

		QPDF& getHandle();
		void subsetFonts();
	public:
		PDF();

		Page& addPage();
		void save(std::filesystem::path path);
	};

}