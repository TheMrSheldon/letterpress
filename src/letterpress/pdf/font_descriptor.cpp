#include <letterpress/pdf/font_descriptor.hpp>

#include <qpdf/QPDFObjectHandle.hh>

#include <assert.h>

using namespace lp::pdf;

FontDescriptor::FontDescriptor(Font& font) : font(font), handle(QPDFObjectHandle::newDictionary()) {
	handle.replaceKey("/Type", QPDFObjectHandle::newName("/FontDescriptor"));

	// TODO: create dynamically
	//handle.replaceKey("/CharSet", "(/A/B/C/D/E/G/H/I/L/M/N/O/P/R/S/T/V/a/c/d/e/fi/five/h/hyphen/i/k/l/m/n/o/p/quoteright/r/s/t/two/v/w/x/y/z)"_qpdf);
}

QPDFObjectHandle& FontDescriptor::getHandle() {
	return handle;
}

bool FontDescriptor::getFontName(Identifier& value) noexcept {
	auto entry = handle.getKey("/FontName");
	return entry.getValueAsName(value.get());
}
void FontDescriptor::setFontName(Identifier name) {
	handle.replaceKey("/FontName", QPDFObjectHandle::newName("/"+name.get()));
}

int FontDescriptor::getFlags() noexcept {
	auto value = handle.getKey("/Flags");
	return value.isNull()? 0 : value.getIntValue();
}
void FontDescriptor::setFlags(int flags) {
	handle.replaceKey("/Flags", QPDFObjectHandle::newInteger(flags));
}

bool FontDescriptor::getBBox(Rectangle& value) noexcept {
	auto entry = handle.getKey("/FontBBox");
	if (entry.isRectangle())
		value = entry.getArrayAsRectangle();
	return false;
}
void FontDescriptor::setBBox(Rectangle bbox) {
	handle.replaceKey("/FontBBox", QPDFObjectHandle::newArray(bbox));
}
void FontDescriptor::setBBox(double minX, double minY, double maxX, double maxY) {
	setBBox(Rectangle(minX, minY, maxX, maxY));
}

void FontDescriptor::setFontFile(QPDFObjectHandle& data) {
	handle.replaceKey("/FontFile", data);
}

double FontDescriptor::getAscent() noexcept {
	auto ascent = handle.getKey("/Ascent");
	return ascent.isNull()? 0 : ascent.getNumericValue();
}
void FontDescriptor::setAscent(double ascent) {
	handle.replaceKey("/Ascent", QPDFObjectHandle::newReal(ascent));
}

double FontDescriptor::getCapHeight() noexcept {
	auto value = handle.getKey("/CapHeight");
	return value.isNull()? 0 : value.getNumericValue();
}
void FontDescriptor::setCapHeight(double capheight) {
	handle.replaceKey("/CapHeight", QPDFObjectHandle::newReal(capheight));
}

double FontDescriptor::getDescent() noexcept {
	auto descent = handle.getKey("/Descent");
	return descent.isNull()? 0 : descent.getNumericValue();
}
void FontDescriptor::setDescent(double descent) {
	handle.replaceKey("/Descent", QPDFObjectHandle::newReal(descent));
}