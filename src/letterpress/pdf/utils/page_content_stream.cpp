#include <letterpress/pdf/utils/page_content_stream.hpp>

#include <letterpress/pdf/page.hpp>

#include <assert.h>

using namespace lp::doc;
using namespace lp::pdf;
using namespace lp::pdf::utils;

PageContentStream::PageContentStream(Page& page, QPDFObjectHandle handle) : page(page), handle(handle) {
	graphicsState.push({});
}

PageContentStream::GraphicsState& PageContentStream::getGraphicsState() noexcept {
	assert(!graphicsState.empty());
	return graphicsState.top();
}

QPDFObjectHandle& PageContentStream::getHandle() { return handle; }

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
		auto kern = font.getKerning(last, glyph);
		if (kern != 0_pt) {
			contentTxt.append(str.str());
			str.str({});
			str.clear();
			contentTxt.append(-kern.getPoint()); /** \todo pt correct? **/
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
PageContentStream& PageContentStream::moveTo(float x, float y) {
	assert(!inTextMode);
	stream.moveTo(x, y); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::lineTo(float x, float y) {
	assert(!inTextMode);
	stream.lineTo(x, y); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::closePath() {
	assert(!inTextMode);
	stream.closePath(); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::appendRect(float posx, float posy, float width, float height) {
	assert(!inTextMode);
	stream.appendRect(posx, posy, width, height); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::stroke() {
	assert(!inTextMode);
	stream.stroke(); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::closeAndStroke() {
	assert(!inTextMode);
	stream.closeAndStroke(); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::fillNonZero() {
	assert(!inTextMode);
	stream.fillNonZero(); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::fillEvenOdd() {
	assert(!inTextMode);
	stream.fillEvenOdd(); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::fillNonZeroAndStroke() {
	assert(!inTextMode);
	stream.fillNonZeroAndStroke(); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::fillEvenOddAndStroke() {
	assert(!inTextMode);
	stream.fillEvenOddAndStroke(); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::closeFillNonZeroAndStroke() {
	assert(!inTextMode);
	stream.closeFillNonZeroAndStroke(); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::closeFillEvenOddAndStroke() {
	assert(!inTextMode);
	stream.closeFillEvenOddAndStroke(); /** \todo update internal state **/
	return *this;
}
PageContentStream& PageContentStream::endPath() {
	assert(!inTextMode);
	stream.endPath(); /** \todo update internal state **/
	return *this;
}

void PageContentStream::flush() {
	handle.replaceStreamData(stream.getContent(), QPDFObjectHandle::newNull(), QPDFObjectHandle::newNull());
}