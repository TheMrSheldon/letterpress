#pragma once

#include <istream>

namespace lp::parser {
	class Visitor {
		virtual void visitCommand(std::string command) = 0;
		virtual void visitCharacter(char32_t codepoint) = 0;
		virtual void visitSpace() = 0;
	};

	enum class TokenType { Command, Space, Character };

	struct Token {
		TokenType type;
		std::string value;
	};

	class Lexer {
	private:
		std::istream& stream;

	public:
		Lexer(std::istream& stream);

		bool next(Token& token);

		// void addActiveSequence(std::string& sequence, std::string command);
	};

	class Parser {
	private:
	public:
		Parser(Lexer& lexer, Visitor& visitor);
	};
}; // namespace lp::parser