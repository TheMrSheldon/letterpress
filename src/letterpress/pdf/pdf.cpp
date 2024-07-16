#include <letterpress/pdf/pdf.hpp>

#include <letterpress/pdf/font.hpp>
#include <letterpress/pdf/page.hpp>

#include <qpdf/QPDF.hh>
#include <qpdf/QPDFPageDocumentHelper.hh>
#include <qpdf/QPDFWriter.hh>

using namespace lp::pdf;

PDF::PDF() { handle.emptyPDF(); }

QPDF& PDF::getHandle() { return handle; }

void PDF::subsetFonts() {
	for (auto&& [_, font] : fonts)
		font->computeSubset();
}

Page& PDF::addPage() { return *pages.emplace_back(std::make_unique<Page>(*this)); }

Font& PDF::registerFont(lp::pdf::utils::FontFilePtr font) {
	auto iterator = fonts.find(font);
	if (iterator == std::end(fonts)) {
		auto& ptr = fonts[font] = std::move(std::make_unique<Font>(*this, font));
		return *ptr;
	}
	return *iterator->second;
}

void PDF::save(std::filesystem::path path) {
	QPDFPageDocumentHelper dh(handle);
	for (auto&& page : pages) {
		page->getContentStream().flush();
		dh.addPage(page->getHandle(), false);
	}

	subsetFonts();

	QPDFWriter writer(handle);
	writer.setOutputFilename(path.c_str());
	writer.write();
}