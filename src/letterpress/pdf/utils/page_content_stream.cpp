#include <letterpress/pdf/utils/page_content_stream.hpp>

#include <letterpress/pdf/page.hpp>

#include <assert.h>

using namespace lp::pdf;
using namespace lp::pdf::utils;

PageContentStream::PageContentStream(Page& page, QPDFObjectHandle handle) : page(page), handle(handle) {
	graphicsState.push({});
}

PageContentStream::GraphicsState& PageContentStream::getGraphicsState() noexcept {
	assert(!graphicsState.empty());
	return graphicsState.top();
}

QPDFObjectHandle& PageContentStream::getHandle() {
	return handle;
}

PageContentStream& PageContentStream::setFont(Font& font, float size) {
	auto& gstate = getGraphicsState();
	gstate.font = &font;
	auto& resources = page.getResources();
	auto fontid = resources.addFont(font);
	stream.setFontAndSize(fontid, size);
	return *this;
}

PageContentStream& PageContentStream::beginText() {
	assert(!inTextMode);
	inTextMode = true;
	stream.beginText();
	return *this;
}

PageContentStream& PageContentStream::endText() {
	assert(inTextMode);
	inTextMode = false;
	stream.endText();
	return *this;
}

PageContentStream& PageContentStream::showKernedText(std::string text) {
	assert(inTextMode);
	auto& gstate = getGraphicsState();
	assert(gstate.font != nullptr);
	auto& font = *gstate.font;
	// Calculate array with strings and adjustments
	lp::pdf::Array contentTxt;
	std::ostringstream str;
	int last = 0;
	for (char c : text) {
		auto glyph = font.getGlyphForChar(c);
		float kern = font.getKerning(last, glyph);
		if (kern != 0) {
			contentTxt.append(str.str());
			str.str({});
			str.clear();
			contentTxt.append(-kern);
		}
		if (c != '\0') {
			str << c;
		} else {
		}
		last = glyph;
	}
	contentTxt.append(str.str());
	// Write to stream
	stream.showTextAdjusted(contentTxt);
	return *this;
}

PageContentStream& PageContentStream::moveText(float tx, float ty) {
	assert(inTextMode);
	stream.moveText(tx, ty);
	return *this;
}

PageContentStream& PageContentStream::setTextLeading(float leading) {
	stream.setTextLeading(leading);
	return *this;
}

PageContentStream& PageContentStream::showText(std::string str) {
	assert(inTextMode);
	stream.showText(str);
	return *this;
}

PageContentStream& PageContentStream::nextLine() {
	assert(inTextMode);
	stream.nextLine();
	return *this;
}

void PageContentStream::flush() {
	handle.replaceStreamData(stream.getContent(), QPDFObjectHandle::newNull(), QPDFObjectHandle::newNull());
}