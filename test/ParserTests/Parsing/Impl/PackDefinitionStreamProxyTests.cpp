#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "Parsing/Impl/PackDefinitionStreamProxy.h"
#include "Parsing/Mock/MockParserLineStream.h"

namespace test::parsing::impl::pack_definition_stream_proxy
{
    void ExpectLine(IParserLineStream* stream, const int lineNumber, const std::string& value)
    {
        auto line = stream->NextLine();
        REQUIRE(line.m_line_number == lineNumber);
        REQUIRE(line.m_line == value);
    }

    TEST_CASE("PackDefinitionStreamProxy: Ensure simple pack directives are working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines
        {
            "hello world",
            "#pragma pack(push, 32)",
            "hello galaxy",
            "#pragma pack(pop)",
            "hello universe"
        };

        MockParserLineStream mockStream(lines);
        PackDefinitionStreamProxy proxy(&mockStream);

        REQUIRE(proxy.GetCurrentPack() == PackDefinitionStreamProxy::DEFAULT_PACK);
        ExpectLine(&proxy, 1, "hello world");
        REQUIRE(proxy.GetCurrentPack() == PackDefinitionStreamProxy::DEFAULT_PACK);
        ExpectLine(&proxy, 3, "hello galaxy");
        REQUIRE(proxy.GetCurrentPack() == 32);
        ExpectLine(&proxy, 5, "hello universe");
        REQUIRE(proxy.GetCurrentPack() == PackDefinitionStreamProxy::DEFAULT_PACK);

        REQUIRE(proxy.Eof());
    }
}