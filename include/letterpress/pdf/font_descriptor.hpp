#pragma once

#include <letterpress/pdf/constants.hpp>
#include <letterpress/pdf/types/identifier.hpp>

#include <qpdf/QPDFObjectHandle.hh>

namespace lp::pdf {
	class Font;

	// Section 5.7 (pp. 456f)
	class FontDescriptor final {
		friend Font;
	private:
		Font& font;
		QPDFObjectHandle handle;

		using Rectangle = QPDFObjectHandle::Rectangle;

		QPDFObjectHandle& getHandle();
	public:
		FontDescriptor(Font& font);

		bool getFontName(Identifier& value) noexcept;
		void setFontName(Identifier name);

		std::string getFontFamily() noexcept;
		void setFontFamily(std::string family);

		FontStretch getStretch() noexcept;
		void setStretch(FontStretch stretch);

		float getWeight() noexcept;
		void setWeight(float weight);
		
		//TODO make enum
		int getFlags() noexcept;
		void setFlags(int flags);

		bool getBBox(Rectangle& value) noexcept;
		void setBBox(Rectangle bbox);
		void setBBox(double minX, double minY, double maxX, double maxY);

		void setFontFile(QPDFObjectHandle& data);

		//
		double getAscent() noexcept;
		void setAscent(double ascent);
		
		double getCapHeight() noexcept;
		void setCapHeight(double capheight);
		
		double getDescent() noexcept;
		void setDescent(double descent);
	};

}