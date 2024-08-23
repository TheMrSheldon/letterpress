#include <letterpress/parser/parser.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

#include <vector>

using Catch::Matchers::RangeEquals;

TEST_CASE("parser", "[parser]") {
	std::istringstream stream("A Hello  \n \n \\world \u2211");
	lp::parser::Lexer lexer(stream);
	lp::parser::Token token;
	REQUIRE(lexer.next(token));
	CHECK(token.type == lp::parser::TokenType::Character);
	CHECK(token.value == "A");

	REQUIRE(lexer.next(token));
	CHECK(token.type == lp::parser::TokenType::Space);

	REQUIRE(lexer.next(token));
	CHECK(token.type == lp::parser::TokenType::Character);
	CHECK(token.value == "H");

	REQUIRE(lexer.next(token));
	CHECK(token.type == lp::parser::TokenType::Character);
	CHECK(token.value == "e");

	REQUIRE(lexer.next(token));
	CHECK(token.type == lp::parser::TokenType::Character);
	CHECK(token.value == "l");

	REQUIRE(lexer.next(token));
	CHECK(token.type == lp::parser::TokenType::Character);
	CHECK(token.value == "l");

	REQUIRE(lexer.next(token));
	CHECK(token.type == lp::parser::TokenType::Character);
	CHECK(token.value == "o");

	REQUIRE(lexer.next(token));
	CHECK(token.type == lp::parser::TokenType::Command);
	CHECK(token.value == "par");

	REQUIRE(lexer.next(token));
	CHECK(token.type == lp::parser::TokenType::Command);
	CHECK(token.value == "world");

	REQUIRE(lexer.next(token));
	CHECK(token.type == lp::parser::TokenType::Space);

	REQUIRE(lexer.next(token));
	CHECK(token.type == lp::parser::TokenType::Character);
	CHECK(token.value == "\u2211");

	CHECK(!lexer.next(token));
}