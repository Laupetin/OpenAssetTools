#include "Menu/AbstractMenuWriter.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    class TestMenuWriter final : public menu::AbstractBaseWriter
    {
    public:
        explicit TestMenuWriter(std::ostream& stream)
            : AbstractBaseWriter(stream)
        {
        }

        using AbstractBaseWriter::CreateScriptTokenList;
        using AbstractBaseWriter::WriteEscapedString;
    };

    TEST_CASE("AbstractMenuWriter: Preserves literal line breaks in script arguments", "[menu][writing]")
    {
        const std::string lineBreak = GENERATE("\n", "\r", "\r\n");
        const auto argument = std::string("onlinegame 0;\txstopprivateparty") + lineBreak;
        const auto script = "\"exec\" \"" + argument + "\" ; ";
        const auto tokens = TestMenuWriter::CreateScriptTokenList(script.c_str());

        REQUIRE(tokens.size() == 3);
        CHECK(tokens[0] == "exec");
        CHECK(tokens[1] == argument);
        CHECK(tokens[2] == ";");

        std::ostringstream output;
        TestMenuWriter writer(output);
        writer.WriteEscapedString(tokens[1]);
        const auto escapedArgument = output.str();

        CHECK(escapedArgument.find('\n') == std::string::npos);
        CHECK(escapedArgument.find('\r') == std::string::npos);
        const auto reloadedTokens = TestMenuWriter::CreateScriptTokenList(escapedArgument.c_str());
        REQUIRE(reloadedTokens.size() == 1);
        CHECK(reloadedTokens[0] == argument);
    }

    TEST_CASE("AbstractMenuWriter: Preserves script command separators across lines", "[menu][writing]")
    {
        const auto tokens = TestMenuWriter::CreateScriptTokenList("\"exec\" \"first\" ;\r\n\"exec\" \"second\" ;");
        const std::vector<std::string> expected{"exec", "first", ";", "exec", "second", ";"};

        CHECK(tokens == expected);
    }
} // namespace
