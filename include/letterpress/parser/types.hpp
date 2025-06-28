#ifndef LETTERPRESS_PARSER_TYPES_HPP
#define LETTERPRESS_PARSER_TYPES_HPP

#include "../scriptengine/object.hpp"

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace lp {
	struct LPValue;
	using LPDict = std::map<std::string, LPValue>;
	using LPArray = std::vector<LPValue>;
	struct LPValue : public std::variant<int, float, std::string, LPDict, LPArray, lp::script::Object, void*> {};
} // namespace lp

#endif