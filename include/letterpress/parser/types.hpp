#ifndef LETTERPRESS_PARSER_TYPES_HPP
#define LETTERPRESS_PARSER_TYPES_HPP

#include <any>
#include <map>
#include <string>

namespace lp {
	using LPValue = std::any;
	using LPDict = std::map<std::string, std::any>;
} // namespace lp

#endif