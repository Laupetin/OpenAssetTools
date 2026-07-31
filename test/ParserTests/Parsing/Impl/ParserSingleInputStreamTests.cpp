#include "Parsing/Impl/ParserSingleInputStream.h"

#include <catch2/catch_test_macros.hpp>
#include <sstream>

namespace test::parsing::impl::parser_single_input_stream
{
    TEST_CASE("ParserSingleInputStream: Strips UTF-8 BOM at start of input", "[parsing][parsingstream]")
    {
        std::istringstream input("\xEF\xBB\xBF"
                                 "first line\nsecond line");
        ParserSingleInputStream stream(input, "input");

        REQUIRE(stream.NextLine().m_line == "first line");
        REQUIRE(stream.NextLine().m_line == "second line");
    }

    TEST_CASE("ParserSingleInputStream: Preserves UTF-8 BOM outside start of input", "[parsing][parsingstream]")
    {
        std::istringstream input("first line\n\xEF\xBB\xBF"
                                 "second line");
        ParserSingleInputStream stream(input, "input");

        REQUIRE(stream.NextLine().m_line == "first line");
        REQUIRE(stream.NextLine().m_line
                == "\xEF\xBB\xBF"
                   "second line");
    }
} // namespace test::parsing::impl::parser_single_input_stream
