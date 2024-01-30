#pragma once

#include "document/document.hpp"

namespace lp {
    struct Page { // Datastructure describing a page
        unsigned width; /**< The page width. The unit for it is at the descretion of the respective driver. */
        unsigned height; /**< The page height. The unit for it is at the descretion of the respective driver. */
        lp::doc::Document::VertList content;
    };

    class Driver {
    private:

    public:
        virtual ~Driver() = default;

        virtual void shipout(const Page& page) = 0;
    };
}