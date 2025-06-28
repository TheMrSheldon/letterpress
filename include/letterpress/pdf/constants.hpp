#pragma once

#include "types/identifier.hpp"

namespace lp::pdf {
	constexpr double inch_to_user = 72.0; /** \todo remove in favor of the Dimension class **/

	enum class Filter { None, FlateDecode };

	const char* to_string(const Filter& filter);

	enum class Operator {
		BeginText,
		EndText,

		SetCharSpacing,
		SetWordSpacing,
		SetTextHorizontalScaling,
		SetTextLeading,
		SetFontAndSize,
		SetTextRenderingMode,
		SetTextRise,

		MoveText,
		MoveTextSetLeading,
		SetMatrix,
		NextLine,

		ShowText,
		ShowTextLine,
		ShowTextLineAndSpace,
		ShowTextAdjusted,

		/** Graphics Operators **/
		/*Page 167 (PDF 2.0)*/
		MoveTo,
		LineTo,
		CurveTo,
		CurveToReplicateInitialPoint,
		CurveToReplicateFinalPoint,
		ClosePath,
		AppendRect,
		/*Page 170 (PDF 2.0)*/
		Stroke,
		CloseAndStroke,
		FillNonZero,
		FillEvenOdd,
		FillNonZeroAndStroke,
		FillEvenOddAndStroke,
		CloseFillNonZeroAndStroke,
		CloseFillEvenOddAndStroke,
		EndPath
	};

	const char* get_opcode(const Operator& op);

	/**
	 * @brief 
	 * 
	 * (p. 402)
	 */
	enum class RenderingMode {
		Fill = 0,
		Stroke = 1,
		FillAndStroke = 2,
		Invisible = 3,
		FillAndAddToClipping = 4,
		StrokeAndAddToClipping = 5,
		FillAndStrokeAndAddToClipping = 6,
		AddToClipping = 7
	};

	/**
	 * @brief 
	 * 
	 * (p. 456)
	 */
	enum class FontStretch {
		UltraCondensed = 0,
		ExtraCondensed = 1,
		Condensed = 2,
		SemiCondensed = 3,
		Normal = 4,
		SemiExpanded = 5,
		Expanded = 6,
		ExtraExpanded = 7,
		UltraExpanded = 8,
	};

	const Identifier to_identifier(const FontStretch& filter);
} // namespace lp::pdf