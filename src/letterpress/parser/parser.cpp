#include <letterpress/parser/parser.hpp>

using namespace lp;

ParserLookAheadIter::value_type ParserLookAheadIter::operator*() const noexcept { return parser->lookAhead(offset); }