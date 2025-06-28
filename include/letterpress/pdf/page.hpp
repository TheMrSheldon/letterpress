#pragma once

#include "font.hpp"
#include "utils/page_content_stream.hpp"
#include "utils/resources.hpp"

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

		double toUserSpace(lp::doc::Dimension length) {
			/* Note: (Tex) Points are exactly 72in. UserUnits are expressed in multiples of 1/72 in (i.e. Tex Points)
			 * (p. 109 \cite PDF20) */
			return length.getPoint() * getUserUnit();
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

} // namespace lp::pdf