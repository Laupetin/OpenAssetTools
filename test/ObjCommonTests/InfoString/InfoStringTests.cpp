#include "InfoString/InfoString.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <sstream>

namespace
{
    TEST_CASE("InfoString: Can parse from stream")
    {
        InfoString info;

        SECTION("without prefix")
        {
            std::istringstream ss(R"(foo\bar\test\value)");
            info.FromStream(ss);
        }
        SECTION("with prefix")
        {
            std::istringstream ss(R"(FOOFILE\foo\bar\test\value)");
            info.FromStream("FOOFILE", ss);
        }
        SECTION("from gdt")
        {
            GdtEntry entry;
            entry.m_properties.emplace("foo", "bar");
            entry.m_properties.emplace("test", "value");
            info.FromGdtProperties(entry);
        }

        REQUIRE(info.HasKey("foo"));
        REQUIRE(info.HasKey("test"));

        REQUIRE(info.GetValueForKey("foo") == "bar");
        REQUIRE(info.GetValueForKey("test") == "value");
    }

    TEST_CASE("InfoString: Can create string")
    {
        InfoString info;
        std::istringstream ss(R"(foo\bar\test\value)");
        info.FromStream(ss);

        SECTION("without prefix")
        {
            REQUIRE(info.ToString() == R"(foo\bar\test\value)");
        }
        SECTION("with prefix")
        {
            REQUIRE(info.ToString("FOOFILE") == R"(FOOFILE\foo\bar\test\value)");
        }
    }

    TEST_CASE("InfoString: Is case insensitive")
    {
        InfoString info;
        std::istringstream ss(R"(foo\bar\test\value)");
        info.FromStream(ss);

        REQUIRE(info.GetValueForKey("FOO") == "bar");
        REQUIRE(info.GetValueForKey("TEST") == "value");
    }
} // namespace
