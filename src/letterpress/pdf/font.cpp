#include <letterpress/pdf/font.hpp>

#include <letterpress/pdf/pdf.hpp>
#include <letterpress/pdf/utils/file_content_provider.hpp>
#include <letterpress/pdf/utils/fontfile.hpp>

using namespace lp::doc;
using namespace lp::pdf;
using namespace lp::pdf::utils;

/** \todo this is not optimal since if a font-object is out of scope, it can't be part of the pdf anymore **/
Font::Font(PDF& pdf, FontFilePtr file)
		: pdf(pdf), file(file), handle(QPDFObjectHandle::newDictionary()), descriptor(*this) {
	handle.replaceKey("/Type", QPDFObjectHandle::newName("/Font"));
	handle.replaceKey("/Subtype", QPDFObjectHandle::newName("/TrueType"));
	handle.replaceKey("/Encoding", QPDFObjectHandle::newName("/WinAnsiEncoding")); //???
	handle.replaceKey("/FontDescriptor", descriptor.getHandle());

	auto name = file->getFamilyName();
	handle.replaceKey("/BaseFont", QPDFObjectHandle::newName("/" + name));
	descriptor.setFontName(Identifier{name});

	descriptor.setAscent(file->getAscent());
	descriptor.setDescent(file->getDescent());
	QPDFObjectHandle::Rectangle rect;
	file->getBoundingBox(rect.llx, rect.lly, rect.urx, rect.ury);
	descriptor.setBBox(rect);

	auto stream = QPDFObjectHandle::newStream(&pdf.getHandle());
	auto filter = Filter::None;
	data = std::make_shared<FileContentProvider>(file->getPath(), filter);
	stream.replaceStreamData(data, QPDFObjectHandle::parse(to_string(filter)), QPDFObjectHandle::newNull());
	descriptor.setFontFile(stream);

	/** \todo feed glyph information **/
	/** \todo remove and replace by dynamicly loaded values **/
	descriptor.setCapHeight(694);
	descriptor.setFlags(4);
}

QPDFObjectHandle& Font::getHandle() { return handle; }

unsigned Font::getGlyphForChar(char32_t c) const { return file->getGlyphForChar(c); }

Dimension Font::getKerning(char32_t leftGlyph, char32_t rightGlyph) const {
	return file->getKerning(leftGlyph, rightGlyph);
}

void Font::addToSubset(unsigned glyph) { subsetGlyphs.push_back(glyph); }

void Font::computeSubset() {
	/** \todo actually calculate only for necessary characters **/
	handle.replaceKey("/FirstChar", QPDFObjectHandle::newInteger(0));
	handle.replaceKey("/LastChar", QPDFObjectHandle::newInteger(255));
	auto widths = QPDFObjectHandle::newArray();
	for (char32_t i = 0; i < 255; i++) {
		auto glyph = file->getGlyphForChar(i);
		// The glyph widths are measured in units in which 1000 units corresponds to 1 unit in text space (p. 414)
		auto advancePt =
				(file->getGlyphInfo(glyph).advanceX * 1000.0f).resolve(1, 0).getPoint(); /** \todo pt correct? **/
		widths.appendItem(QPDFObjectHandle::newInteger(static_cast<int>(advancePt)));
	}
	handle.replaceKey("/Widths", widths);
}