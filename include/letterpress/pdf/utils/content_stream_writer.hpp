#pragma once

#include "../constants.hpp"
#include "../types/array.hpp"
#include "../types/identifier.hpp"

#include <qpdf/QPDFObjectHandle.hh>

#include <sstream>
#include <string>

namespace lp::pdf::utils {
	class ContentStreamWriter final {
	private:
		using Operator = lp::pdf::Operator;
		std::ostringstream content;

		static constexpr char StringStartChar = '(';
		static constexpr char StringEndChar = ')';
		static constexpr char ArrayStartChar = '[';
		static constexpr char ArrayEndChar = ']';
		static constexpr char IdentifierStart = '/';
		static constexpr char SepChar = ' ';

		template <Operator op>
		void writeOperator() noexcept {
			content << lp::pdf::get_opcode(op) << SepChar;
		}

		void write(const std::string& str) noexcept {
			/** \todo escape parenthesis **/
			content << StringStartChar << str << StringEndChar << SepChar;
		}

		void write(Identifier ident) noexcept { content << IdentifierStart << ident.get() << SepChar; }

		void write(float num) noexcept { content << num << SepChar; }

		void write(lp::pdf::Array array) noexcept {
			content << ArrayStartChar;
			for (const auto& element : array.get())
				std::visit([this](auto e) { write(e); }, element);
			content << ArrayEndChar << SepChar;
		}

		template <Operator op, typename... Args>
		ContentStreamWriter& write(Args&&... args) noexcept {
			((write(args)), ...);
			writeOperator<op>();
			return *this;
		}

	public:
		// Page 398
		/**
		 * @brief Set the character spacing, Tc, to charSpace, which is a number expressed in unscaled text space units.
		 * Character spacing is used by the Tj, TJ, and ' operators. Initial value: 0.
		 * 
		 * @param charSpace 
		 * @return ** ContentStreamWriter& 
		 */
		ContentStreamWriter& setCharSpacing(float charSpace) { return write<Operator::SetCharSpacing>(charSpace); }
		/**
		 * @brief Set the word spacing, Tw, to wordSpace, which is a number expressed in unscaled text space units. Word
		 * spacing is used by the Tj, TJ, and ' operators. Initial value: 0. 
		 * 
		 * @param wordSpace 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& setWordSpacing(float wordSpace) { return write<Operator::SetWordSpacing>(wordSpace); }
		/**
		 * @brief Set the horizontal scaling, Th, to (scale ÷ 100). scale is a number specifying the percentage of the
		 * normal width. Initial value: 100 (normal width).
		 * 
		 * @param scale 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& setTextHorizontalScaling(float scale) {
			return write<Operator::SetTextHorizontalScaling>(scale);
		}
		/**
		 * @brief Set the text leading, Tl, to leading, which is a number expressed in unscaled text space units. Text
		 * leading is used only by the T*, ', and " operators. Initial value: 0. 
		 * 
		 * @param leading 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& setTextLeading(float leading) { return write<Operator::SetTextLeading>(leading); }
		/**
		 * @brief Set the text font, Tf, to font and the text font size, Tfs, to size. font is the name of a font
		 * resource in the Font subdictionary of the current resource dictionary; size is a number representing a scale
		 * factor. There is no initial value for either font or size; they must be specified explicitly by using Tf
		 * before any text is shown. 
		 * 
		 * @param fontname 
		 * @param size 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& setFontAndSize(Identifier font, float size) noexcept {
			return write<Operator::SetFontAndSize>(font, size);
		}
		/**
		 * @brief Set the text rendering mode, Tmode, to render, which is an integer. Initial value: 0.
		 * 
		 * @param mode 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& setTextRenderingMode(RenderingMode mode) {
			return write<Operator::SetTextRenderingMode>(static_cast<int>(mode));
		}
		/**
		 * @brief Set the text rise, Trise, to rise, which is a number expressed in unscaled text space units. Initial
		 * value: 0.
		 * 
		 * @param mode 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& setTextRise(float rise) { return write<Operator::SetTextRise>(rise); }

		// Page 405
		/**
		 * @brief Begin a text object, initializing the text matrix, Tm, and the text line matrix, Tlm, to
		 * the identity matrix. Text objects cannot be nested; a second BT cannot appear before
		 * an ET. 
		 * 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& beginText() noexcept { return write<Operator::BeginText>(); }
		/**
		 * @brief End a text object, discarding the text matrix
		 * 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& endText() noexcept { return write<Operator::EndText>(); }

		// Page 406
		/**
		 * @brief Move to the start of the next line, offset from the start of the current line by
		 * (tx , ty ). tx and ty are numbers expressed in unscaled text space units. More precisely, this operator performs the following assignments:
		 * 
		 * @param tx 
		 * @param ty 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& moveText(float tx, float ty) noexcept { return write<Operator::MoveText>(tx, ty); }
		/**
		 * @brief Move to the start of the next line, offset from the start of the current line by
		 * (tx , ty ). As a side effect, this operator sets the leading parameter in the text state.
		 * This operator has the same effect as the following code:
		 * −ty TL tx ty Td
		 * 
		 * @param tx 
		 * @param ty 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& moveTextSetLeading(float tx, float ty) noexcept {
			return write<Operator::MoveTextSetLeading>(tx, ty);
		}

		/**
		 * @brief Set the text matrix, Tm, and the text line matrix, Tlm:
		 * The operands are all numbers, and the initial value for Tm and Tlm is the identity
		 * matrix, [1 0 0 1 0 0]. Although the operands specify a matrix, they are passed
		 * to Tm as six separate numbers, not as an array.
		 * The matrix specified by the operands is not concatenated onto the current text
		 * matrix, but replaces it. 
		 * 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& setMatrix(float a, float b, float c, float d, float e, float f) noexcept {
			return write<Operator::SetMatrix>(a, b, c, d, e, f);
		}

		/**
		 * @brief Move to the start of the next line. This operator has the same effect as the code
		 * 0 Tl Td
		 * where Tl is the current leading parameter in the text state.
		 * 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& nextLine() noexcept { return write<Operator::NextLine>(); }

		// Page 407
		/**
		 * @brief Show a text string.
		 * 
		 * @param text 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& showText(std::string text) noexcept { return write<Operator::ShowText>(text); }
		/**
		 * @brief Move to the next line and show a text string. This operator has the same effect as
		 * the code
		 * T*
		 * string Tj
		 * 
		 * @param text 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& showTextLine(std::string text) noexcept { return write<Operator::ShowTextLine>(text); }
		/**
		 * @brief Move to the next line and show a text string, using aw as the word spacing and ac
		 * as the character spacing (setting the corresponding parameters in the text state).
		 * aw and ac are numbers expressed in unscaled text space units. This operator has
		 * the same effect as the following code:
		 * aw Tw
		 * ac Tc
		 * string '
		 * 
		 * @param aw 
		 * @param ac 
		 * @param text 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& showTextLineAndSpace(float aw, float ac, std::string text) noexcept {
			return write<Operator::ShowTextLineAndSpace>(aw, ac, text);
		}
		/*TODO*/
		/**
		 * @brief Show one or more text strings, allowing individual glyph positioning (see implementation note 58 in
		 * Appendix H). Each element of array can be a string or anumber. If the element is a string, this operator
		 * shows the string. If it is a number, the operator adjusts the text position by that amount; that is, it
		 * translates the text matrix, Tm. The number is expressed in thousandths of a unit of text space (see Section
		 * 5.3.3, “Text Space Details,” and implementation note 59 in Appendix H). This amount is subtracted from the
		 * current horizontal or vertical coordinate, depending on the writing mode. In the default coordinate system, a
		 * positive adjustment has the effect of moving the next glyph painted either to the left or down by the given
		 * amount. Figure 5.11 shows an example of the effect of passing offsets to TJ. 
		 * 
		 * @param text 
		 * @return ContentStreamWriter& 
		 */
		ContentStreamWriter& showTextAdjusted(lp::pdf::Array param) noexcept {
			return write<Operator::ShowTextAdjusted>(param);
		}

		std::string getContent() const noexcept { return content.str(); }
	};
} // namespace lp::pdf::utils