#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Mock/MockParserLineStream.h"

namespace test::parsing::impl::comment_removing_stream_proxy
{
    TEST_CASE("CommentRemovingStreamProxy: Ensure simple single line comment is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines
        {
            "// hello",
            "prefix // test",
            "t//est"
        };

        MockParserLineStream mockStream(lines);
        CommentRemovingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_line.empty());
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 2);
            REQUIRE(line.m_line == "prefix ");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(line.m_line == "t");
        }

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("CommentRemovingStreamProxy: Ensure single line comment expands to next line on backslash", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines
        {
            "// hello\\",
            "this should still be a comment",
            "this should not be a comment anymore"
        };

        MockParserLineStream mockStream(lines);
        CommentRemovingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_line.empty());
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 2);
            REQUIRE(line.m_line.empty());
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(line.m_line == "this should not be a comment anymore");
        }

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("CommentRemovingStreamProxy: Ensure single line comment expands to next line on backslash and is repeatable", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines
        {
            "// hello\\",
            "this should still be a comment \\",
            "this as well",
            "this not anymore"
        };

        MockParserLineStream mockStream(lines);
        CommentRemovingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_line.empty());
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 2);
            REQUIRE(line.m_line.empty());
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(line.m_line.empty());
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 4);
            REQUIRE(line.m_line == "this not anymore");
        }

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("CommentRemovingStreamProxy: Ensure backslash must be last character to expand single line comment to next line", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines
        {
            "// hello\\",
            "this should still be a comment \\ ",
            "this not anymore"
        };

        MockParserLineStream mockStream(lines);
        CommentRemovingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_line.empty());
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 2);
            REQUIRE(line.m_line.empty());
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(line.m_line == "this not anymore");
        }

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("CommentRemovingStreamProxy: Ensure simple multiline comment works on one line", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines
        {
            "hello/* hell*/ world",
            "/*this should be a comment*/",
            "Hello /*asdf*/",
            "/*asdf*/World"
        };

        MockParserLineStream mockStream(lines);
        CommentRemovingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_line == "hello world");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 2);
            REQUIRE(line.m_line.empty());
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(line.m_line == "Hello ");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 4);
            REQUIRE(line.m_line == "World");
        }

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("CommentRemovingStreamProxy: Ensure simple multiline comment works over multiple lines", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines
        {
            "hello/* hell",
            " hell*/ world/*nope",
            "notatall",
            "hehe*/xd"
        };

        MockParserLineStream mockStream(lines);
        CommentRemovingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_line == "hello");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 2);
            REQUIRE(line.m_line == " world");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 3);
            REQUIRE(line.m_line.empty());
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 4);
            REQUIRE(line.m_line == "xd");
        }

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("CommentRemovingStreamProxy: Can have multiple comment blocks in one line", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines
        {
            "Hello/* lovely*/ world and/* beautiful*/ universe",
            "Hello/* lovely*/ world// and beautiful universe",
        };

        MockParserLineStream mockStream(lines);
        CommentRemovingStreamProxy proxy(&mockStream);

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 1);
            REQUIRE(line.m_line == "Hello world and universe");
        }

        {
            auto line = proxy.NextLine();
            REQUIRE(line.m_line_number == 2);
            REQUIRE(line.m_line == "Hello world");
        }

        REQUIRE(proxy.Eof());
    }
}
