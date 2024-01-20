#include <letterpress/pdf/constants.hpp>

using namespace lp::pdf;

const char* lp::pdf::to_string(const Filter& filter) {
	static const char* strings[] = {
		"null",
		"/FlateDecode",
	};
	static_assert(static_cast<int>(Filter::None) == 0);
	static_assert(static_cast<int>(Filter::FlateDecode) == 1);
	return strings[static_cast<int>(filter)];
}

const char* lp::pdf::get_opcode(const Operator& op) {
	static const char* opcodes[] = {
		"BT", "ET",
		/*Page 398*/
		"Tc", "Tw", "Tz", "TL", "Tf", "Tr", "Ts",
		/*Page 406*/
		"Td", "TD", "Tm", "T*",
		/*Page 407*/
		"Tj", "'", "\"", "TJ",
	};
	static_assert(static_cast<int>(Operator::BeginText) == 0);
	static_assert(static_cast<int>(Operator::EndText) == 1);
	/* p. 398 */
	static_assert(static_cast<int>(Operator::SetCharSpacing) == 2);
	static_assert(static_cast<int>(Operator::SetWordSpacing) == 3);
	static_assert(static_cast<int>(Operator::SetTextHorizontalScaling) == 4);
	static_assert(static_cast<int>(Operator::SetTextLeading) == 5);
	static_assert(static_cast<int>(Operator::SetFontAndSize) == 6);
	static_assert(static_cast<int>(Operator::SetTextRenderingMode) == 7);
	static_assert(static_cast<int>(Operator::SetTextRise) == 8);
	/* p. 406 */
	static_assert(static_cast<int>(Operator::MoveText) == 9);
	static_assert(static_cast<int>(Operator::MoveTextSetLeading) == 10);
	static_assert(static_cast<int>(Operator::SetMatrix) == 11);
	static_assert(static_cast<int>(Operator::NextLine) == 12);
	/* p. 407 */
	static_assert(static_cast<int>(Operator::ShowText) == 13);
	static_assert(static_cast<int>(Operator::ShowTextLine) == 14);
	static_assert(static_cast<int>(Operator::ShowTextLineAndSpace) == 15);
	static_assert(static_cast<int>(Operator::ShowTextAdjusted) == 16);
	return opcodes[static_cast<int>(op)];
}