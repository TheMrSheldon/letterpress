#ifndef LETTERPRESS_PARSER_LPVISITOR_HPP
#define LETTERPRESS_PARSER_LPVISITOR_HPP

#include "types.hpp"

#include <string>
#include <vector>

namespace lp {
	class LPVisitor {
	public:
		virtual void visitImport(const std::string& moduleName) = 0;
		virtual void visitDoctype(const std::string& doctype, const LPDict& args) = 0;

		virtual void visitCommand(const std::string& command) = 0;
		virtual void visitWhitespace() = 0;
		virtual void visitCharacter(char32_t c) = 0;
	};
} // namespace lp

#endif