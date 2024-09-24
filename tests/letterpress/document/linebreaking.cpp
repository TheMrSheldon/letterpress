#include <letterpress/document/boxes.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

#include <vector>

using Catch::Matchers::RangeEquals;

TEST_CASE("linebreaking", "[linebreaking]") {
	lp::doc::HBox box;
	box << lp::doc::HBox();
}