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
		/** \todo **/
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

		/**************************************************************************************************************/
		/* GRAPHICS OPERATIONS                                                                                        */
		/**************************************************************************************************************/

		/**
		 * @brief Begin a new subpath by moving the current point to coordinates (x, y), omitting any connecting line
		 * segment. If the previous path construction operator in the current path was also m, the new m overrides it;
		 * no vestige of the previous m operation remains in the path. (p. 167 \cite PDF20)
		 * 
		 * @param x 
		 * @param y 
		 * @return 
		 */
		ContentStreamWriter& moveTo(float x, float y) noexcept { return write<Operator::MoveTo>(x, y); }
		/**
		 * @brief Append a straight line segment from the current point to the point (x, y). The new current point shall
		 * be (x, y) (p. 167 \cite PDF20)
		 * 
		 * @param x 
		 * @param y 
		 * @return 
		 */
		ContentStreamWriter& lineTo(float x, float y) noexcept { return write<Operator::LineTo>(x, y); }
		/**
		 * @brief Append a cubic Bézier curve to the current path. The curve shall extend from the current point to the
		 * point (x3, y3), using (x1, y1 ) and (x2, y2 ) as the Bézier control points (see 8.5.2.2, "Cubic Bézier
		 * curves"). The new current point shall be (x3, y3 ). (p. 167 \cite PDF20)
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 * @param x3 
		 * @param y3 
		 * @return 
		 */
		ContentStreamWriter& curveTo(float x1, float y1, float x2, float y2, float x3, float y3) noexcept {
			return write<Operator::CurveTo>(x1, y1, x2, y2, x3, y3);
		}
		/**
		 * @brief Append a cubic Bézier curve to the current path. The curve shall extend from the current point to the
		 * point (x3, y3 ), using the current point and (x2, y2 ) as the Bézier control points (see 8.5.2.2, "Cubic
		 * Bézier curves"). The new current point shall be (x3, y3 ). (p. 167 \cite PDF20)
		 * 
		 * @param x2 
		 * @param y2 
		 * @param x3 
		 * @param y3 
		 * @return 
		 */
		ContentStreamWriter& curveToReplicateInitialPoint(float x2, float y2, float x3, float y3) noexcept {
			return write<Operator::CurveToReplicateInitialPoint>(x2, y2, x3, y3);
		}
		/**
		 * @brief Append a cubic Bézier curve to the current path. The curve shall extend from the current point to the
		 * point (x3, y3 ), using (x1, y1 ) and (x3, y3 ) as the Bézier control points (see 8.5.2.2, "Cubic Bézier
		 * curves"). The new current point shall be (x3, y3 ). (p. 167 \cite PDF20)
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x3 
		 * @param y3 
		 * @return 
		 */
		ContentStreamWriter& curveToReplicateFinalPoint(float x1, float y1, float x3, float y3) noexcept {
			return write<Operator::CurveToReplicateFinalPoint>(x1, y1, x3, y3);
		}
		/**
		 * @brief Close the current subpath by appending a straight line segment from the current point to the starting
		 * point of the subpath. If the current subpath is already closed, h shall do nothing. This operator terminates
		 * the current subpath. Appending another segment to the current path shall begin a new subpath, even if the new
		 * segment begins at the endpoint reached by the h operation. (p. 167 \cite PDF20)
		 * 
		 * @return 
		 */
		ContentStreamWriter& closePath() noexcept { return write<Operator::ClosePath>(); }
		/**
		 * @brief Append a rectangle to the current path as a complete subpath, with lower-left corner (x, y) and
		 * dimensions width and height in user space. (p. 167 \cite PDF20)
		 * @details The operation: `x y width height re` is equivalent to:
		 * ```
		 * x y m
		 * (x + width) y l
		 * (x + width) (y + height) l
		 * x (y + height) l+
		 * h
		 * ```
		 * 
		 * @param x 
		 * @param y 
		 * @param width 
		 * @param height 
		 * @return 
		 */
		ContentStreamWriter& appendRect(float x, float y, float width, float height) noexcept {
			return write<Operator::AppendRect>(x, y, width, height);
		}

		/**
		 * @brief Stroke the path. (p. 170 \cite PDF20)
		 * 
		 * @return 
		 */
		ContentStreamWriter& stroke() noexcept { return write<Operator::Stroke>(); }

		/**
		 * @brief Close and stroke the path. This operator shall have the same effect as the sequence h S. (p. 170
		 * \cite PDF20)
		 * 
		 * @return 
		 */
		ContentStreamWriter& closeAndStroke() noexcept { return write<Operator::CloseAndStroke>(); }

		ContentStreamWriter& fillNonZero() noexcept { return write<Operator::FillNonZero>(); }

		ContentStreamWriter& fillEvenOdd() noexcept { return write<Operator::FillEvenOdd>(); }

		ContentStreamWriter& fillNonZeroAndStroke() noexcept { return write<Operator::FillNonZeroAndStroke>(); }

		ContentStreamWriter& fillEvenOddAndStroke() noexcept { return write<Operator::FillEvenOddAndStroke>(); }

		ContentStreamWriter& closeFillNonZeroAndStroke() noexcept {
			return write<Operator::CloseFillNonZeroAndStroke>();
		}

		ContentStreamWriter& closeFillEvenOddAndStroke() noexcept {
			return write<Operator::CloseFillNonZeroAndStroke>();
		}

		ContentStreamWriter& endPath() noexcept { return write<Operator::ClosePath>(); }

		std::string getContent() const noexcept { return content.str(); }
	};
} // namespace lp::pdf::utils