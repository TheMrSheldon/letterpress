#include <letterpress/pdf/pdf.hpp>

#include <letterpress/pdf/font.hpp>

#include <qpdf/QPDF.hh>
#include <qpdf/QPDFPageDocumentHelper.hh>
#include <qpdf/QPDFWriter.hh>

using namespace lp::pdf;

PDF::PDF() {
	handle.emptyPDF();
}

QPDF& PDF::getHandle() {
	return handle;
}

void PDF::subsetFonts() {
	std::vector<std::reference_wrapper<Font>> toSubset;
	std::swap(toSubset, fontsToSubset);
	for (auto&& font : toSubset)
		font.get().computeSubset();
}

Page& PDF::addPage() {
	return *pages.emplace_back(std::make_unique<Page>(*this));
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