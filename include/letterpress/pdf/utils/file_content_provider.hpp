#pragma once

#include <qpdf/QPDFObjectHandle.hh>

#include <string>

#include <letterpress/pdf/constants.hpp>

namespace lp::pdf::utils {
	class FileContentProvider : public QPDFObjectHandle::StreamDataProvider {
	private:
		std::string path;
		Filter filter;
	public:
		FileContentProvider(std::string path, Filter filter);
		virtual ~FileContentProvider() = default;
		virtual void provideStreamData(QPDFObjGen const&, Pipeline* pipeline) override;
	};
}