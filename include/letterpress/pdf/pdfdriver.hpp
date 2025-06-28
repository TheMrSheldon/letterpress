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
		explicit PDFDriver(std::filesystem::path outfile);
		virtual ~PDFDriver();

		virtual void shipout(const lp::doc::VBox& page) override;

		virtual void setAuthor(std::optional<std::string> author) override;
		virtual void setDate(std::optional<std::string> date) override;
		virtual void setTitle(std::optional<std::string> title) override;
		virtual void setDescription(std::optional<std::string> description) override;
		virtual void setKeywords(std::vector<std::string> keywords) override;
	};
} // namespace lp
