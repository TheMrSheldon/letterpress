#pragma once

#include "../font.hpp"
#include "content_stream_writer.hpp"

#include <qpdf/QPDFObjectHandle.hh>

#include <stack>
#include <string>

namespace lp::pdf {
	class Page;
}

namespace lp::pdf::utils {
	class PageContentStream final {
		friend Page;
	private:
		Page& page;
		ContentStreamWriter stream;
		struct GraphicsState {
			Font* font;
			//strokingColorSpace
			//nonStrokingColorSpace
		};
		std::stack<GraphicsState> graphicsState;
		QPDFObjectHandle handle;

		bool inTextMode;

		GraphicsState& getGraphicsState() noexcept;

		QPDFObjectHandle& getHandle();
	public:
		PageContentStream(Page& page, QPDFObjectHandle handle);

		PageContentStream& setFont(Font& font, float size);

		PageContentStream& saveGraphicsState();
		PageContentStream& restoreGraphicsState();

		PageContentStream& beginText();
		PageContentStream& endText();

		PageContentStream& showKernedText(std::string str);
		PageContentStream& moveText(float tx, float ty);
		PageContentStream& showText(std::string str);
		PageContentStream& nextLine();
		PageContentStream& setTextLeading(float leading);

		void flush();

		ContentStreamWriter& getStreamWriter() noexcept { return stream; }
	};
}