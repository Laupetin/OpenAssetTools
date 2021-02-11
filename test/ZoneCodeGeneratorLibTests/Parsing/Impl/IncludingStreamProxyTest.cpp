#include <catch2/catch.hpp>

#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Mock/MockParserLineStream.h"

namespace test::parsing
{
    TEST_CASE("IncludingStreamProxy: Ensure simple include is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines
        {
            "Hello world",
            "#include \"ASDF.txt\"",
            "and bye"
        };

        const std::vector<std::string> asdf
        {
            "Hello galaxy"
        };

        MockParserLineStream mockStream(lines);
        mockStream.AddIncludeLines("ASDF.txt", asdf);

        IncludingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_filename.get() == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "Hello world");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_filename.get() == "ASDF.txt");
            REQUIRE(line.m_line == "Hello galaxy");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(line.m_filename.get() == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "and bye");
        }

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("IncludingStreamProxy: Ensure simple include with angle brackets is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines
        {
            "Hello world",
            "#include <ASDF.txt>",
            "and bye"
        };

        const std::vector<std::string> asdf
        {
            "Hello galaxy"
        };

        MockParserLineStream mockStream(lines);
        mockStream.AddIncludeLines("ASDF.txt", asdf);

        IncludingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_filename.get() == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "Hello world");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_filename.get() == "ASDF.txt");
            REQUIRE(line.m_line == "Hello galaxy");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(line.m_filename.get() == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "and bye");
        }

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("IncludingStreamProxy: Ensure can have spaces before include directive", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines
        {
            "Hello world",
            "   #include     \"ASDF.txt\"     ",
            "and bye"
        };

        const std::vector<std::string> asdf
        {
            "Hello galaxy"
        };

        MockParserLineStream mockStream(lines);
        mockStream.AddIncludeLines("ASDF.txt", asdf);

        IncludingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_filename.get() == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "Hello world");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_filename.get() == "ASDF.txt");
            REQUIRE(line.m_line == "Hello galaxy");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(line.m_filename.get() == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "and bye");
        }

        REQUIRE(proxy.Eof());
    }
}
