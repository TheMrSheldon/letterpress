#include <letterpress/pdf/utils/resources.hpp>

#include <letterpress/pdf/page.hpp>

using namespace lp::pdf;
using namespace lp::pdf::utils;

Resources::Resources() : handle(QPDFObjectHandle::newDictionary()), fonts(QPDFObjectHandle::newDictionary()) {
	handle.replaceKey("/Font", fonts);
}

QPDFObjectHandle& Resources::getHandle() {
	return handle;
}

Identifier Resources::addFont(Font& font) {
	auto dict = fonts.getDictAsMap();
	for (auto& [key, value] : dict) {
		if (value.getObjectID() == font.getHandle().getObjectID()) {
			return Identifier(key.substr(1));
		}
	}
	int number = 1;
	for (;fonts.hasKey("/F"+std::to_string(number)); ++number);
	fonts.replaceKey("/F"+std::to_string(number), font.getHandle());
	return Identifier("F"+std::to_string(number));
}