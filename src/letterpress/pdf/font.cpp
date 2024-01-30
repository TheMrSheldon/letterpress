#include <letterpress/pdf/font.hpp>

#include <letterpress/pdf/pdf.hpp>
#include <letterpress/pdf/utils/file_content_provider.hpp>

using namespace lp::pdf;
using namespace lp::pdf::utils;

/** \todo this is not optimal since if a font-object is out of scope, it can't be part of the pdf anymore **/
Font::Font(PDF& pdf, std::string path, std::string afmPath) : pdf(pdf), file(path, afmPath), handle(QPDFObjectHandle::newDictionary()), descriptor(*this) {
	handle.replaceKey("/Type", QPDFObjectHandle::newName("/Font"));
	handle.replaceKey("/Subtype", QPDFObjectHandle::newName("/TrueType"));
	handle.replaceKey("/Encoding", QPDFObjectHandle::newName("/WinAnsiEncoding")); //???
	handle.replaceKey("/FontDescriptor", descriptor.getHandle());

	auto name = file.getFamilyName();
	handle.replaceKey("/BaseFont", QPDFObjectHandle::newName("/"+name));
	descriptor.setFontName({name});

	descriptor.setAscent(file.getAscent());
	descriptor.setDescent(file.getDescent());
	QPDFObjectHandle::Rectangle rect;
	file.getBoundingBox(rect.llx, rect.lly, rect.urx, rect.ury);
	descriptor.setBBox(rect);

	auto stream = QPDFObjectHandle::newStream(&pdf.getHandle());
	auto filter = Filter::FlateDecode;
	data = std::make_shared<FileContentProvider>(path, filter);
	stream.replaceStreamData(data, QPDFObjectHandle::parse(to_string(filter)), QPDFObjectHandle::newNull());
	descriptor.setFontFile(stream);

	//TODO: feed glyph information
	// TODO: remove and replace by dynamicly loaded values
	descriptor.setCapHeight(694);
	descriptor.setFlags(4);
}

QPDFObjectHandle& Font::getHandle() {
	return handle;
}

unsigned Font::getGlyphForChar(char c) const {
	return file.getGlyphForChar(c);
}

float Font::getKerning(unsigned leftGlyph, unsigned rightGlyph) const {
	return file.getKerning(leftGlyph, rightGlyph);
}

void Font::addToSubset(unsigned glyph) {
	subsetGlyphs.push_back(glyph);
}

void Font::computeSubset() {
	/** \todo actually calculate only for necessary characters **/
	handle.replaceKey("/FirstChar", QPDFObjectHandle::newInteger(0));
	handle.replaceKey("/LastChar", QPDFObjectHandle::newInteger(255));
	auto widths = QPDFObjectHandle::newArray();
	for (int i = 0; i < 255; i++) {
		auto glyph = file.getGlyphForChar((char)i);
		// The glyph widths are measured in units in which 1000 units corresponds to 1 unit in text space (p. 414)
		widths.appendItem(QPDFObjectHandle::newInteger(static_cast<int>(file.getGlyphInfo(glyph).advanceX * 1000.0f)));
	}
	handle.replaceKey("/Widths", widths);
}