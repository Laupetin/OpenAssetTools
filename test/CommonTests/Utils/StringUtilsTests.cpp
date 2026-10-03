#include "Utils/StringUtils.h"

#include <catch2/catch_test_macros.hpp>

static_assert(utils::StringEqualsIgnoreCase("", ""));
static_assert(utils::StringEqualsIgnoreCase("Hello World", "hELLO wORLD"));
static_assert(!utils::StringEqualsIgnoreCase("Hello", "Hello World"));
static_assert(!utils::StringEqualsIgnoreCase("[", "{"));

TEST_CASE("StringUtils: StringEqualsIgnoreCase", "[stringutils]")
{
    SECTION("matches strings that only differ in case")
    {
        REQUIRE(utils::StringEqualsIgnoreCase("mp_rust", "MP_Rust"));
        REQUIRE(utils::StringEqualsIgnoreCase("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "abcdefghijklmnopqrstuvwxyz"));
    }

    SECTION("does not match different strings")
    {
        REQUIRE_FALSE(utils::StringEqualsIgnoreCase("mp_rust", "mp_rust2"));
        REQUIRE_FALSE(utils::StringEqualsIgnoreCase("@", "`"));
    }
}
