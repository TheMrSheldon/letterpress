#ifndef LETTERPRESS_PARSER_LPPARSER_HPP
#define LETTERPRESS_PARSER_LPPARSER_HPP

#include "./lpvisitor.hpp"
#include "./parser.hpp"

#include <any>
#include <cassert>
#include <map>
#include <vector>

namespace lp {

	class LPParser final : GenericParser {
	private:
		static const std::regex EndPreamble;
		static const std::regex Import;
		static const std::regex Doctype;
		static const std::regex Identifier;
		static const std::regex Number;
		static const std::regex QuotedString;
		static const std::regex Par; /** \todo generalize to active sequence **/

		LPVisitor& visitor;

		void skipWhitespaces() {
			skipWhile([](const char32_t& c) { return std::isspace(c); });
		}

		constexpr static void rtrim(std::string& s) noexcept {
			s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
					s.end());
		}

	public:
		LPParser(std::istream& stream, LPVisitor& visitor) : GenericParser(stream), visitor(visitor) {}

		std::string readIdentifier() noexcept {
			size_t len = lookAheadMatches(Identifier);
			assert(len != 0); /** \todo handle more gracefully **/
			return readNext(len);
		}
		std::string readQuotedString() noexcept {
			size_t len = lookAheadMatches(QuotedString);
			assert(len != 0); /** \todo handle more gracefully **/
			advance();		  // pop opening "
			auto value = readNext(len - 2);
			advance(); // pop closing "
			return value;
		}
		template <class Pred>
		LPValue readValue(Pred isDelimiter) noexcept {
			switch (lookAhead(0)) {
			case '[':
				return readArray();
			case '{':
				return readDict();
			case '\"':
				return readQuotedString();
			default:
				skipWhitespaces();
				size_t lastIdx = 0;
				for (; !isDelimiter(lookAhead(lastIdx)); ++lastIdx)
					; // read until the prodivded delimiter
				std::string value = readNext(lastIdx);
				rtrim(value);
				if (std::regex_match(value, Number))
					return (float)std::atof(value.c_str());
				return value;
			};
		}
		LPDict readDict() noexcept {
			LPDict dict;
			if (advance() != '{')
				abort(); /** \todo handle more gracefully **/
			skipWhitespaces();
			while (lookAhead(0) != '}') {
				auto key = readIdentifier();
				skipWhitespaces();
				if (advance() != ':')
					abort(); /** \todo handle more gracefully **/
				skipWhitespaces();
				auto value = readValue([](char32_t c) { return c == '}' || c == ','; });
				skipWhitespaces();
				dict[key] = value;
				if (lookAhead(0) != '}' && lookAhead(0) != ',')
					abort(); /** \todo handle more gracefully **/
				if (lookAhead(0) == ',')
					advance();
				skipWhitespaces();
			}
			advance(); // pop the }
			return dict;
		}
		std::vector<std::any> readArray() noexcept {
			std::vector<std::any> array;
			if (advance() != '[')
				abort(); /** \todo handle more gracefully **/
			skipWhitespaces();
			while (lookAhead(0) != ']') {
				array.push_back(readValue([](char32_t c) { return c == ']' || c == ','; }));
				skipWhitespaces();
				if (lookAhead(0) != ']' && lookAhead(0) != ',')
					abort(); /** \todo handle more gracefully **/
				if (lookAhead(0) == ',')
					advance();
				skipWhitespaces();
			}
			advance(); // pop the ]
			return array;
		}

		void parseFile() {
			parsePreamble();
			parseContent();
		}
		void parsePreamble() {
			skipWhitespaces();
			size_t len;
			while ((len = lookAheadMatches(EndPreamble)) == 0) {
				if (len = lookAheadMatches(Import)) {
					advance(len);
					skipWhitespaces();
					visitor.visitImport(readIdentifier());
				} else if (len = lookAheadMatches(Doctype)) {
					advance(len);
					skipWhitespaces();
					auto doctype = readIdentifier();
					skipWhitespaces();
					auto args = readDict();
					visitor.visitDoctype(doctype, args);
				} else {
					/** \todo handle error **/
					abort();
				}
				skipWhitespaces();
			}
			advance(len); // pop the end-of-preamble sequence
		}
		void parseContent() {
			char32_t cur;
			while ((cur = lookAhead(0)) != eof) {
				size_t len;
				if (len = lookAheadMatches(Par)) { /** \todo generalize to active sequences **/
					visitor.visitCommand("par", {});
					advance(len);
				} else {
					advance();
					if (cur == '\\') {
						auto command = readIdentifier();
						std::vector<LPValue> args;
						while (lookAhead(0) == '{') {
							advance();
							args.emplace_back(readValue([](char32_t c) { return c == '}'; }));
							if (advance() != '}')
								abort();
						}
						visitor.visitCommand(command, args);
					} else if (std::isspace(cur)) {
						visitor.visitWhitespace();
					} else {
						visitor.visitCharacter(cur);
					}
				}
			}
		}
	};

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
} // namespace lp

#endif