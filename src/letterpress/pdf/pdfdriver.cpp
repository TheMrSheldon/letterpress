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


template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

void PDFDriver::writeVBox(lp::pdf::Page& page, const lp::doc::VBox& vbox) {
	for (auto&& elem : vbox.content) {
		std::visit(overloaded{
			[this, &page](const lp::doc::VBox& vbox) { writeVBox(page, vbox); },
			[this, &page](const lp::doc::HBox& hbox) { writeHBox(page, hbox); },
			[this, &page](const lp::doc::Glue& glue) { /** \todo implement **/ }
		}, elem);
	}
}

void PDFDriver::writeHBox(lp::pdf::Page& page, const lp::doc::HBox& hbox) {
	auto& stream = page.getContentStream();
	lp::pdf::Array array;
	std::string text;
	float width = 0;
	for (auto&& he : hbox.content) {
		if (const auto val = std::get_if<lp::doc::Glyph>(&he)) {
			text += cpp20_codepoint_to_utf8(val->charcode);
			width += val->width;
		} else if (const auto val = std::get_if<lp::doc::Glue>(&he)) {
			array.append(text);
			// The number shall be expressed in thousandths of a unit of text space
			// (PDF 2.0 Table 107: Text-showing operators)
			//array.append(-width/1000-val->idealwidth*100);
			array.append(-val->idealwidth);
			text = "";
			width = 0;
		} else {
			throw std::runtime_error("Unexpected datatype");
		}
	}
	if (!text.empty())
		array.append(text);
	/** \todo remove hardcoded 1.2*lineskip**/
	stream.getStreamWriter().showTextAdjusted(array).moveText(0, (-12)*1.2);
}

void PDFDriver::shipout(const lp::doc::VBox& page) {
    auto& pdfpage = pdf.addPage();
	int width = (int)pdfpage.mmToUserSpace(page.width);
	int height = (int)pdfpage.mmToUserSpace(page.height);
    pdfpage.setMediaBox(0, 0, width, height);

    /*auto& font1 = pdf.addFont("res/fonts/computer-modern/cmunrm.ttf");
	auto& font2 = pdf.addFont("res/fonts/LatinmodernmathRegular.otf");
	auto& font3 = pdf.addFont("res/fonts/baskervaldx/type1/Baskervaldx-Reg.pfb", "res/fonts/baskervaldx/afm/Baskervaldx-Reg.afm");*/
	auto fontfile = std::make_shared<utils::FontFile>("res/fonts/computer-modern/cmunrm.ttf");
	auto font1 = pdf.registerFont(fontfile);
	
	auto& stream = pdfpage.getContentStream();
	stream.setFont(font1, 12);
	stream.beginText().setTextLeading(24).moveText(72, height-72);
	writeVBox(pdfpage, page);
	stream.endText();
}