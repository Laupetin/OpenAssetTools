#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Mock/MockParserLineStream.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace test::parsing::impl::including_stream_proxy
{
    TEST_CASE("IncludingStreamProxy: Ensure simple include is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{"Hello world", "#include \"ASDF.txt\"", "and bye"};

        const std::vector<std::string> asdf{"Hello galaxy"};

        MockParserLineStream mockStream(lines);
        mockStream.AddIncludeLines("ASDF.txt", asdf);

        IncludingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(*line.m_filename == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "Hello world");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(*line.m_filename == "ASDF.txt");
            REQUIRE(line.m_line == "Hello galaxy");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(*line.m_filename == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "and bye");
        }

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("IncludingStreamProxy: Ensure simple include with angle brackets is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{"Hello world", "#include <ASDF.txt>", "and bye"};

        const std::vector<std::string> asdf{"Hello galaxy"};

        MockParserLineStream mockStream(lines);
        mockStream.AddIncludeLines("ASDF.txt", asdf);

        IncludingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(*line.m_filename == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "Hello world");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(*line.m_filename == "ASDF.txt");
            REQUIRE(line.m_line == "Hello galaxy");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(*line.m_filename == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "and bye");
        }

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("IncludingStreamProxy: Ensure can have spaces before include directive", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{"Hello world", "   #include     \"ASDF.txt\"     ", "and bye"};

        const std::vector<std::string> asdf{"Hello galaxy"};

        MockParserLineStream mockStream(lines);
        mockStream.AddIncludeLines("ASDF.txt", asdf);

        IncludingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(*line.m_filename == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "Hello world");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(*line.m_filename == "ASDF.txt");
            REQUIRE(line.m_line == "Hello galaxy");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(*line.m_filename == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "and bye");
        }

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("IncludingStreamProxy: Ensure pragma once prevents including the same file more than once", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{"Hello world", "#include \"ASDF.txt\"", "#include \"ASDF.txt\"", "and bye"};

        const std::vector<std::string> asdf{"#pragma once", "Hello galaxy"};

        MockParserLineStream mockStream(lines);
        mockStream.AddIncludeLines("ASDF.txt", asdf);

        IncludingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(*line.m_filename == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "Hello world");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 2);
            REQUIRE(*line.m_filename == "ASDF.txt");
            REQUIRE(line.m_line == "Hello galaxy");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 4);
            REQUIRE(*line.m_filename == MockParserLineStream::MOCK_FILENAME);
            REQUIRE(line.m_line == "and bye");
        }

        REQUIRE(proxy.Eof());
    }
} // namespace test::parsing::impl::including_stream_proxy
