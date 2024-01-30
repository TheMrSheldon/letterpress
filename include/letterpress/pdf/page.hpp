#pragma once

#include "utils/page_content_stream.hpp"
#include "utils/resources.hpp"
#include "font.hpp"

#include <qpdf/QPDFPageObjectHelper.hh>

#include <vector>

namespace lp::pdf {

	class Page final {
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

		double mmToUserSpace(double length) {
			auto factor = lp::pdf::mm_to_inch * lp::pdf::inch_to_user * getUserUnit();
			return length * factor;
		}

		/**
		 * @brief Returns the user unit for converting between inches and user space coordinates in multiples of 1/72
		 * inch.
		 * @details The default value is 1.0 denoting that 1 user space unit is 1/72 inches.
		 **/
		double getUserUnit();

		/**
		 * @brief Sets the page's size through specifying the x- and y-coordinates of the lower left (minx, miny) and
		 * upper right (maxx, maxy) corner.
		 * @details The coordinates are measured in default user space units (p. 53 of the pdf-manual).
		 **/
		void setMediaBox(int minx, int miny, int maxx, int maxy);

		/**
		 * @brief Sets the page's crop box, i.e., the region of the page that is actually printed and contains content.
		 * @details The cropbox is a box within the media box and nothing outside the crop box is drawn. The coordinates
		 * are in default user space units (p. 54 of the pdf-manual).
		 **/
		void setCropBox(int minx, int miny, int maxx, int maxy);

		/**
		 * @brief Unsers the crop page's crop box.
		 **/
		void removeCropBox();
	};

}