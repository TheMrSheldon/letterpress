#include <letterpress/pdf/page.hpp>

#include <letterpress/pdf/pdf.hpp>

using namespace lp::pdf;
using namespace lp::pdf::utils;


Page::Page(PDF& pdf) : stream(*this, QPDFObjectHandle::newStream(&pdf.getHandle())), handle("<< /Type /Page >>"_qpdf) {
	auto obj = handle.getObjectHandle();
	obj.replaceKey("/Contents", stream.getHandle());
	obj.replaceKey("/Resources", resources.getHandle());
}

QPDFPageObjectHelper& Page::getHandle() {
	return handle;
}

PageContentStream& Page::getContentStream() {
	return stream;
}
utils::Resources& Page::getResources() {
	return resources;
}

void Page::setMediaBox(int minx, int miny, int maxx, int maxy) {
	using Rect = QPDFObjectHandle::Rectangle;
	handle.getObjectHandle().replaceKey(
		"/MediaBox", QPDFObjectHandle::newFromRectangle(Rect(minx, miny, maxx, maxy))
	);
}