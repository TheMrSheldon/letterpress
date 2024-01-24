#include <letterpress/document/hyphenation.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

#include <vector>

using Catch::Matchers::RangeEquals;

TEST_CASE("hyphenation", "[hyphenation]") {
    lp::doc::Hyphenation hyph;
    CHECK_THAT(hyph.hyphenate("abcbc"), RangeEquals(std::vector{"abcbc"}));
    hyph.addPattern(".a3b");
    CHECK_THAT(hyph.hyphenate("abcbc"), RangeEquals(std::vector{"a", "bcbc"}));
    hyph.addPattern("b3c");
    CHECK_THAT(hyph.hyphenate("abcbc"), RangeEquals(std::vector{"a","b","cb","c"}));
    hyph.addPattern("b4c.");
    CHECK_THAT(hyph.hyphenate("abcbc"), RangeEquals(std::vector{"a","b","cbc"}));
}