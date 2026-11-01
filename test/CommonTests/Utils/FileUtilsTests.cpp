#include "Utils/FileUtils.h"

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

TEST_CASE("FileUtils: Parses paths string in specified order", "[fileutils]")
{
    std::vector<std::string> paths;

    REQUIRE(utils::ParsePathsString("zone_raw/b;zone_raw/a;\"raw;c\"", paths));
    REQUIRE(paths == std::vector<std::string>{"zone_raw/b", "zone_raw/a", "raw;c"});
}

TEST_CASE("FileUtils: Parses paths string keeping first of duplicate paths", "[fileutils]")
{
    std::vector<std::string> paths{"raw"};

    REQUIRE(utils::ParsePathsString("zone_raw;raw;zone_raw;lang", paths));
    REQUIRE(paths == std::vector<std::string>{"raw", "zone_raw", "lang"});
}
