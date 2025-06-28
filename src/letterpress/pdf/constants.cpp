#include <letterpress/pdf/constants.hpp>

using namespace lp::pdf;

const char* lp::pdf::to_string(const Filter& filter) {
	static constexpr const char* strings[] = {
			"null",
			"/FlateDecode",
	};
	static_assert(strings[static_cast<int>(Filter::None)] == std::string_view{"null"});
	static_assert(strings[static_cast<int>(Filter::FlateDecode)] == std::string_view{"/FlateDecode"});
	return strings[static_cast<int>(filter)];
}

const char* lp::pdf::get_opcode(const Operator& op) {
	static constexpr const char* opcodes[]{
			"BT", "ET",
			/*Page 398*/
			"Tc", "Tw", "Tz", "TL", "Tf", "Tr", "Ts",
			/*Page 406*/
			"Td", "TD", "Tm", "T*",
			/*Page 407*/
			"Tj", "'", "\"", "TJ",
			/*Page 167 (PDF 2.0)*/
			"m", "l", "c", "v", "y", "h", "re",
			/*Page 170 (PDF 2.0)*/
			"S", "s", "f", "f*", "B", "B*", "b", "b*", "n"
	};
	static_assert(opcodes[static_cast<int>(Operator::BeginText)] == std::string_view{"BT"});
	static_assert(opcodes[static_cast<int>(Operator::EndText)] == std::string_view{"ET"});
	/* p. 398 */
	static_assert(opcodes[static_cast<int>(Operator::SetCharSpacing)] == std::string_view{"Tc"});
	static_assert(opcodes[static_cast<int>(Operator::SetWordSpacing)] == std::string_view{"Tw"});
	static_assert(opcodes[static_cast<int>(Operator::SetTextHorizontalScaling)] == std::string_view{"Tz"});
	static_assert(opcodes[static_cast<int>(Operator::SetTextLeading)] == std::string_view{"TL"});
	static_assert(opcodes[static_cast<int>(Operator::SetFontAndSize)] == std::string_view{"Tf"});
	static_assert(opcodes[static_cast<int>(Operator::SetTextRenderingMode)] == std::string_view{"Tr"});
	static_assert(opcodes[static_cast<int>(Operator::SetTextRise)] == std::string_view{"Ts"});
	/* p. 406 */
	static_assert(opcodes[static_cast<int>(Operator::MoveText)] == std::string_view{"Td"});
	static_assert(opcodes[static_cast<int>(Operator::MoveTextSetLeading)] == std::string_view{"TD"});
	static_assert(opcodes[static_cast<int>(Operator::SetMatrix)] == std::string_view{"Tm"});
	static_assert(opcodes[static_cast<int>(Operator::NextLine)] == std::string_view{"T*"});
	/* p. 407 */
	static_assert(opcodes[static_cast<int>(Operator::ShowText)] == std::string_view{"Tj"});
	static_assert(opcodes[static_cast<int>(Operator::ShowTextLine)] == std::string_view{"'"});
	static_assert(opcodes[static_cast<int>(Operator::ShowTextLineAndSpace)] == std::string_view{"\""});
	static_assert(opcodes[static_cast<int>(Operator::ShowTextAdjusted)] == std::string_view{"TJ"});
	/*Page 167 (PDF 2.0)*/
	static_assert(opcodes[static_cast<int>(Operator::MoveTo)] == std::string_view{"m"});
	static_assert(opcodes[static_cast<int>(Operator::LineTo)] == std::string_view{"l"});
	static_assert(opcodes[static_cast<int>(Operator::CurveTo)] == std::string_view{"c"});
	static_assert(opcodes[static_cast<int>(Operator::CurveToReplicateInitialPoint)] == std::string_view{"v"});
	static_assert(opcodes[static_cast<int>(Operator::CurveToReplicateFinalPoint)] == std::string_view{"y"});
	static_assert(opcodes[static_cast<int>(Operator::ClosePath)] == std::string_view{"h"});
	static_assert(opcodes[static_cast<int>(Operator::AppendRect)] == std::string_view{"re"});
	/*Page 170 (PDF 2.0)*/
	static_assert(opcodes[static_cast<int>(Operator::Stroke)] == std::string_view{"S"});
	static_assert(opcodes[static_cast<int>(Operator::CloseAndStroke)] == std::string_view{"s"});
	static_assert(opcodes[static_cast<int>(Operator::FillNonZero)] == std::string_view{"f"});
	static_assert(opcodes[static_cast<int>(Operator::FillEvenOdd)] == std::string_view{"f*"});
	static_assert(opcodes[static_cast<int>(Operator::FillNonZeroAndStroke)] == std::string_view{"B"});
	static_assert(opcodes[static_cast<int>(Operator::FillEvenOddAndStroke)] == std::string_view{"B*"});
	static_assert(opcodes[static_cast<int>(Operator::CloseFillNonZeroAndStroke)] == std::string_view{"b"});
	static_assert(opcodes[static_cast<int>(Operator::CloseFillEvenOddAndStroke)] == std::string_view{"b*"});
	static_assert(opcodes[static_cast<int>(Operator::EndPath)] == std::string_view{"n"});
	return opcodes[static_cast<int>(op)];
}