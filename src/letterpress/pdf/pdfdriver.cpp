#include <letterpress/pdf/pdfdriver.hpp>

#include <letterpress/pdf/page.hpp>

using namespace lp;
using namespace lp::pdf;

PDFDriver::PDFDriver(std::filesystem::path outfile) : outfile(outfile), pdf() {}

PDFDriver::~PDFDriver() {
    pdf.save(outfile);
}

inline
std::string cpp20_codepoint_to_utf8(char32_t cp) // C++20 Sandard
{
    using codecvt_32_8_type = std::codecvt<char32_t, char8_t, std::mbstate_t>;

    struct codecvt_utf8
    : public codecvt_32_8_type
        { codecvt_utf8(std::size_t refs = 0): codecvt_32_8_type(refs) {} };

    char8_t utf8[4];
    char8_t* end_of_utf8;

    char32_t const* from = &cp;

    std::mbstate_t mbs;
    codecvt_utf8 ccv;

    if(ccv.out(mbs, from, from + 1, from, utf8, utf8 + 4, end_of_utf8))
        throw std::runtime_error("bad conversion");

    return {reinterpret_cast<char*>(utf8), reinterpret_cast<char*>(end_of_utf8)};
}

void PDFDriver::shipout(const Page& page) {
    auto& pdfpage = pdf.addPage();
	int width = (int)pdfpage.mmToUserSpace(page.width);
	int height = (int)pdfpage.mmToUserSpace(page.height);
    pdfpage.setMediaBox(0, 0, width, height);

    auto& font1 = pdf.addFont("res/fonts/computer-modern/cmunrm.ttf");
	auto& font2 = pdf.addFont("res/fonts/LatinmodernmathRegular.otf");
	auto& font3 = pdf.addFont("res/fonts/baskervaldx/type1/Baskervaldx-Reg.pfb", "res/fonts/baskervaldx/afm/Baskervaldx-Reg.afm");

	auto& stream = pdfpage.getContentStream();
	stream.setFont(font1, 12);

	stream.beginText().setTextLeading(24).moveText(72, height-72);
	for (auto&& elem : page.content) {
		if (elem.type() == typeid(lp::doc::Document::HorizList)) {
			lp::pdf::Array array;
			auto horizList = std::any_cast<lp::doc::Document::HorizList>(elem);
			for (auto&& he : horizList) {
				if (const auto val = std::get_if<char32_t>(&he)) {
					array.append(cpp20_codepoint_to_utf8(*val));
				} else if (const auto val = std::get_if<lp::doc::Document::Glue>(&he)) {
					array.append(" ");
					array.append(val->amount);
				} else {
					throw std::runtime_error("Unexpected datatype");
				}
			}
			/** \todo remove hardcoded 1.2*lineskip**/
			stream.getStreamWriter().showTextAdjusted(array).moveText(0, (-12)*1.2);
		} else {
			throw std::runtime_error("Unexpected datatype");
		}
	}
	stream.endText();
}