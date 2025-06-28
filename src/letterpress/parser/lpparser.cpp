#include <letterpress/parser/lpparser.hpp>

using namespace lp;

const std::regex LPParser::EndPreamble{"==+\\n", std::regex_constants::nosubs | std::regex_constants::optimize};
const std::regex LPParser::Import{"import\\b", std::regex_constants::nosubs | std::regex_constants::optimize};
const std::regex LPParser::Doctype{"doctype\\b", std::regex_constants::nosubs | std::regex_constants::optimize};
const std::regex LPParser::Identifier{
		"[A-Za-z][A-Za-z0-9]*", std::regex_constants::nosubs | std::regex_constants::optimize
};
const std::regex LPParser::Number{
		"[0-9]+|([0-9]*\\.[0-9]+)", std::regex_constants::nosubs | std::regex_constants::optimize
};
const std::regex LPParser::QuotedString{
		"\\\"(\\\\\\\"|[^\"])*\\\"", std::regex_constants::nosubs | std::regex_constants::optimize
};
const std::regex LPParser::Par{"\\n\\s*\\n", std::regex_constants::nosubs | std::regex_constants::optimize};
