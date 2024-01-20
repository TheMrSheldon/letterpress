#pragma once

#include "utils/page_content_stream.hpp"
#include "utils/resources.hpp"
#include "font.hpp"

#include <qpdf/QPDFPageObjectHelper.hh>

#include <vector>

namespace lp::pdf {

	class Page {
		friend class PDF;
		using PageContentStream = lp::pdf::utils::PageContentStream;
	private:
		PageContentStream stream;
		utils::Resources resources;
		QPDFPageObjectHelper handle;


		Page(const Page& other) = delete;
		Page(Page&& other) = delete;
		Page& operator=(const Page& other) = delete;

		QPDFPageObjectHelper& getHandle();
	public:
		Page(class PDF& pdf);


		PageContentStream& getContentStream();
		utils::Resources& getResources();

		void setMediaBox(int minx, int miny, int maxx, int maxy);
	};

}