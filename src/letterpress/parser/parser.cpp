#include <letterpress/parser/parser.hpp>

#include <algorithm>

using Lexer = lp::parser::Lexer;
using Token = lp::parser::Token;
using TokenType = lp::parser::TokenType;

Lexer::Lexer(std::istream& stream) : stream(stream) {}

bool Lexer::next(Token& token) {
	/** \todo support utf-8 **/
	if (!stream.good())
		return false;
	if (std::isspace(stream.peek())) {
		token.type = TokenType::Space;
		bool prevLinebreak = false;
		while (std::isspace(stream.peek())) {
			if (stream.get() == '\n') {
				if (prevLinebreak) {
					token.type = TokenType::Command;
					token.value = "par";
				}
				prevLinebreak = true;
			}
		}
	} else if (stream.peek() == '\\') {
		token.type = TokenType::Command;
		token.value.clear();
		stream.get();
		while (std::isalpha(stream.peek()))
			token.value += (char)stream.get();
	} else {
		token.type = TokenType::Character;
		token.value.resize(1);
		stream.read(token.value.data(), 1);
	}
	return true;
}