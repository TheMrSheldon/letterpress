#pragma once

#include "../driver.hpp"
#include "pdf.hpp"

#include <filesystem>

namespace lp {
    class PDFDriver final : public Driver {
    private:
        std::filesystem::path outfile;
        lp::pdf::PDF pdf;

    public:
        PDFDriver(std::filesystem::path outfile);
        virtual ~PDFDriver();

        virtual void shipout(const Page& page) override;
    };
}

