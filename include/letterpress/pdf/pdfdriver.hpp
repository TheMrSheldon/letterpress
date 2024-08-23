#pragma once

#include "../driver.hpp"
#include "pdf.hpp"

#include "../document/boxes.hpp"

#include <filesystem>

namespace lp {
	class PDFDriver final : public Driver {
	private:
		std::filesystem::path outfile;
		lp::pdf::PDF pdf;

		void writeVBox(lp::pdf::Page& page, const lp::doc::VBox& vbox);
		void writeHBox(lp::pdf::Page& page, const lp::doc::HBox& hbox);
		void createGraphic();

	public:
		PDFDriver(std::filesystem::path outfile);
		virtual ~PDFDriver();

		virtual void shipout(const lp::doc::VBox& page) override;
	};
} // namespace lp
