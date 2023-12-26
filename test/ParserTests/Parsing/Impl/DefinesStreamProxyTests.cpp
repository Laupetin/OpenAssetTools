#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Mock/MockParserLineStream.h"
#include "Parsing/ParsingException.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace Catch::Matchers;

namespace test::parsing::impl::defines_stream_proxy
{
    void ExpectLine(IParserLineStream* stream, const int lineNumber, const std::string& value)
    {
        auto line = stream->NextLine();
        REQUIRE(line.m_line_number == lineNumber);
        REQUIRE(line.m_line == value);
    }

    void ExpectErrorInLine(IParserLineStream* stream, const int lineNumber, const int columnNumber)
    {
        REQUIRE_THROWS_MATCHES(
            stream->NextLine(), ParsingException, MessageMatches(ContainsSubstring("L" + std::to_string(lineNumber) + ":" + std::to_string(columnNumber))));
    }

    TEST_CASE("DefinesStreamProxy: Ensure simple define and positive ifdef is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define ASDF",
            "#ifdef ASDF",
            "Hello World",
            "#endif",
            "Hello Galaxy",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "Hello World");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "Hello Galaxy");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure simple define and negative ifdef is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define ASDF",
            "#ifdef NONO",
            "Hello World",
            "#endif",
            "Hello Galaxy",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "Hello Galaxy");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure simple define and positive ifndef is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define ASDF",
            "#ifndef NONO",
            "Hello World",
            "#endif",
            "Hello Galaxy",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "Hello World");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "Hello Galaxy");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure simple define and negative ifndef is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define ASDF",
            "#ifndef ASDF",
            "Hello World",
            "#endif",
            "Hello Galaxy",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "Hello Galaxy");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure else is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define ASDF",
            "#ifdef NONO",
            "Hello World1",
            "#else",
            "Hello World2",
            "#endif",
            "Hello Galaxy",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "Hello World2");
        ExpectLine(&proxy, 6, "");
        ExpectLine(&proxy, 7, "Hello Galaxy");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure nested ifdef is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define ASDF",
            "#ifdef ASDF",
            "#ifdef NONO",
            "Hello World1",
            "#else",
            "Hello World2",
            "#endif",
            "#else",
            "#ifdef ASDF",
            "Hello World3",
            "#else",
            "Hello World4",
            "#endif",
            "#endif",
            "Hello Galaxy",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "");
        ExpectLine(&proxy, 6, "Hello World2");
        ExpectLine(&proxy, 7, "");
        ExpectLine(&proxy, 8, "");
        ExpectLine(&proxy, 9, "");
        ExpectLine(&proxy, 10, "");
        ExpectLine(&proxy, 11, "");
        ExpectLine(&proxy, 12, "");
        ExpectLine(&proxy, 13, "");
        ExpectLine(&proxy, 14, "");
        ExpectLine(&proxy, 15, "Hello Galaxy");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure undef is working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define ASDF",
            "#ifdef ASDF",
            "Hello World",
            "#endif",
            "#undef ASDF",
            "#ifdef ASDF",
            "Hello World",
            "#endif",
            "Hello Galaxy",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "Hello World");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "");
        ExpectLine(&proxy, 6, "");
        ExpectLine(&proxy, 7, "");
        ExpectLine(&proxy, 8, "");
        ExpectLine(&proxy, 9, "Hello Galaxy");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure undef does not undefine everything", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define ASDF",
            "#ifdef ASDF",
            "Hello World",
            "#endif",
            "#undef NONO",
            "#ifdef ASDF",
            "Hello World",
            "#endif",
            "Hello Galaxy",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "Hello World");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "");
        ExpectLine(&proxy, 6, "");
        ExpectLine(&proxy, 7, "Hello World");
        ExpectLine(&proxy, 8, "");
        ExpectLine(&proxy, 9, "Hello Galaxy");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure simple defines are working", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define ASDF LOL",
            "ASDF",
            "A ASDF B",
            "ASDF B",
            "A ASDF",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "LOL");
        ExpectLine(&proxy, 3, "A LOL B");
        ExpectLine(&proxy, 4, "LOL B");
        ExpectLine(&proxy, 5, "A LOL");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure defines can be surrounded by symbols", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define ASDF LOL",
            "!ASDF%",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "!LOL%");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can use multiple defines in one line", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define A Hello",
            "#define B world",
            "A my dear B!",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "Hello my dear world!");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure defines in disabled block are ignored", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#ifdef LOLO",
            "#define hello world",
            "#endif",
            "hello",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "hello");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure undefs in disabled block are ignored", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define hello world",
            "#ifdef LOLO",
            "#undef hello",
            "#endif",
            "hello",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "world");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can define name with underscores and digits", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define __int16 short",
            "unsigned __int16 value;",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "unsigned short value;");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure define can render parameters", "[parsing][parsingstream]")
    {
        DefinesStreamProxy::Define define("helloworld", "hello universe");

        std::vector<std::string> parameterNames({"universe"});
        define.IdentifyParameters(parameterNames);

        std::vector<std::string> parameterValues({"mr moneyman"});
        REQUIRE(define.Render(parameterValues) == "hello mr moneyman");
    }

    TEST_CASE("DefinesStreamProxy: Ensure define can render parameters in middle of symbols", "[parsing][parsingstream]")
    {
        DefinesStreamProxy::Define define("helloworld", "alignas(x)");

        std::vector<std::string> parameterNames({"x"});
        define.IdentifyParameters(parameterNames);

        std::vector<std::string> parameterValues({"1337"});
        REQUIRE(define.Render(parameterValues) == "alignas(1337)");
    }

    TEST_CASE("DefinesStreamProxy: Ensure can add define with parameters", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define test(x) alignas(x)",
            "struct test(1337) test_struct",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "struct alignas(1337) test_struct");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can use parameter multiple times", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define test(x) x|x|x|x",
            "struct test(1337) test_struct",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "struct 1337|1337|1337|1337 test_struct");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can use parameterized define in between symbols", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define test(x) x|x|x|x",
            "%test(5)%",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "%5|5|5|5%");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can define multiple parameters", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define test(a1, a2, a3) a1 + a2 = a3",
            "make calc test(1, 2, 3);",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "make calc 1 + 2 = 3;");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can define multiple parameters without spacing", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define test(a1,a2,a3) a1+a2=a3",
            "make calc test(1,2,3);",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "make calc 1+2=3;");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can define parameters with underscore", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define test(test_parameter) this is test_parameter",
            "Apparently test(a very cool text);",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "Apparently this is a very cool text;");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure simple if is working with truthy value", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#if 1",
            "Hello World",
            "#endif",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "Hello World");
        ExpectLine(&proxy, 3, "");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure simple if is working with non-truthy value", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#if 0",
            "Hello World",
            "#endif",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure simple if is working with calculated values", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#if 0 || 1",
            "Hello World",
            "#endif",
            "#if 0 && 1",
            "Hello World",
            "#endif",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "Hello World");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "");
        ExpectLine(&proxy, 6, "");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can handle defined operator with defined definition", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff 1",
            "#if defined(someStuff)",
            "Hello World",
            "#endif",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "Hello World");
        ExpectLine(&proxy, 4, "");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can handle defined operator with undefined definition", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff 1",
            "#if defined(someStuff) && defined(thisIsNotDefined)",
            "Hello World",
            "#endif",
            "#if defined(thisIsNotDefined)",
            "Hello World",
            "#endif",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "");
        ExpectLine(&proxy, 6, "");
        ExpectLine(&proxy, 7, "");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can use elif", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff 1",
            "#if defined(someStuff) && defined(thisIsNotDefined)",
            "Hello World",
            "#elif 1",
            "Hello Universe",
            "#elif 1",
            "Hello Galaxy",
            "#endif",
            "#if defined(thisIsNotDefined)",
            "Hello World",
            "#endif",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "Hello Universe");
        ExpectLine(&proxy, 6, "");
        ExpectLine(&proxy, 7, "");
        ExpectLine(&proxy, 8, "");
        ExpectLine(&proxy, 9, "");
        ExpectLine(&proxy, 10, "");
        ExpectLine(&proxy, 11, "");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure elif does not work when if was true", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff 1",
            "#if defined(someStuff)",
            "Hello World",
            "#elif 1",
            "Hello Universe",
            "#endif",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "Hello World");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "");
        ExpectLine(&proxy, 6, "");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can use else when no elif matched", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff 1",
            "#if 0",
            "Hello World",
            "#elif 0",
            "Hello Universe",
            "#else",
            "Hello Galaxy",
            "#endif",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "");
        ExpectLine(&proxy, 6, "");
        ExpectLine(&proxy, 7, "Hello Galaxy");
        ExpectLine(&proxy, 8, "");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can use parenthesis in parameters values", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff(param1) Hello param1 World",
            "someStuff(A sentence with (parenthesis) and stuff)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "Hello A sentence with (parenthesis) and stuff World");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can use comma in parenthesis in parameters values", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff(param1) Hello param1 World",
            "someStuff(A sentence with (parenthesis and a , character) and stuff)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "Hello A sentence with (parenthesis and a , character) and stuff World");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can use comma in square brackets in parameters values", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff(param1) Hello param1 World",
            "someStuff(A sentence with [brackets and a , character] and stuff)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "Hello A sentence with [brackets and a , character] and stuff World");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure can use comma in curly braces in parameters values", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff(param1) Hello param1 World",
            "someStuff(A sentence with {braces and a , character} and stuff)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "Hello A sentence with {braces and a , character} and stuff World");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure throws error on unclosed parenthesis in params", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff(param1) Hello param1 World",
            "someStuff(A sentence with [brackets and a , character and stuff)",
            "someStuff(A sentence with {braces and a , character and stuff)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectErrorInLine(&proxy, 2, 64);
        ExpectErrorInLine(&proxy, 3, 62);

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure throws error on parenthesis in params closed with wrong equivalent", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff(param1) Hello param1 World",
            "someStuff(A sentence with (parenthesis and a , character] and stuff)",
            "someStuff(A sentence with [brackets and a , character} and stuff)",
            "someStuff(A sentence with {braces and a , character) and stuff)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectErrorInLine(&proxy, 2, 57);
        ExpectErrorInLine(&proxy, 3, 54);
        ExpectErrorInLine(&proxy, 4, 52);

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Ensure defines can go over multiple lines", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define someStuff(param1) Hello param1 World \\",
            "and hello universe",
            "someStuff(lovely)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "Hello lovely World and hello universe");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Macro definition can span multiple lines when used with backslash", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define testMacro( \\",
            "  a, \\",
            "  b, \\",
            "  c) a + b - c",
            "testMacro(1, 2, 3)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "1 + 2 - 3");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Macro definition that has unclosed parameters throws an error", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define testMacro(",
            "  a,",
            "  b,",
            "  c) a + b - c",
            "testMacro(1, 2, 3)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectErrorInLine(&proxy, 1, 19);
    }

    TEST_CASE("DefinesStreamProxy: Macro usages can span multiple lines if they have args", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define testMacro(a, b, c) Hello a, this is b. Lets meet at c!",
            "testMacro(",
            "Peter,",
            "Anna,",
            "the cinema",
            ")",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "");
        ExpectLine(&proxy, 6, "Hello Peter, this is Anna. Lets meet at the cinema!");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Can use second macro after multi-line macro", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define LOL HAHA",
            "#define testMacro(a, b) a likes b",
            "testMacro(",
            "Peter,",
            "Anna",
            ") LOL funny",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "");
        ExpectLine(&proxy, 6, "Peter likes Anna HAHA funny");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Can use second multi-line macro after multi-line macro", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define LOL HAHA",
            "#define testMacro(a, b) a likes b",
            "testMacro(",
            "Peter,",
            "Anna",
            ") and testMacro(",
            "Anna,",
            "Peter",
            ")",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "");
        ExpectLine(&proxy, 4, "");
        ExpectLine(&proxy, 5, "");
        ExpectLine(&proxy, 6, "Peter likes Anna and ");
        ExpectLine(&proxy, 7, "");
        ExpectLine(&proxy, 8, "");
        ExpectLine(&proxy, 9, "Anna likes Peter");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Can use strinizing operator", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define testMacro(a) #a",
            "testMacro(Hello)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "\"Hello\"");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Can use strinizing operator inside sample code", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define testMacro(a) System.out.println(#a)",
            "testMacro(Hello)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "System.out.println(\"Hello\")");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Can use token-pasting operator with identifier", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define testMacro(a) Hello##a",
            "testMacro(World)",
            "testMacro(5)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "HelloWorld");
        ExpectLine(&proxy, 3, "Hello5");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Can use token-pasting operator with string", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define testMacro(a) \"Hello\"##a",
            "testMacro(\"World\")",
            "testMacro(\"5\")",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "\"HelloWorld\"");
        ExpectLine(&proxy, 3, "\"Hello5\"");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Token-pasting operator ignores whitespace", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define glue(a, b) a   ##      b",
            "glue(\"Hello\", \"World\")",
            "testMacro(Hello, 5)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "\"HelloWorld\"");
        ExpectLine(&proxy, 3, "Hello5");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Can use token-pasting operator with string and stringization outside macro", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define s(t) #t",
            "#define testMacro(a) \"Hello\" ## a",
            "testMacro(s(World))",
            "testMacro(s(5))",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "\"HelloWorld\"");
        ExpectLine(&proxy, 4, "\"Hello5\"");

        REQUIRE(proxy.Eof());
    }

    TEST_CASE("DefinesStreamProxy: Can use token-pasting operator with string and stringization inside macro", "[parsing][parsingstream]")
    {
        const std::vector<std::string> lines{
            "#define s(t) #t",
            "#define testMacro(a) \"Hello\" ## s(a)",
            "testMacro(World)",
            "testMacro(5)",
        };

        MockParserLineStream mockStream(lines);
        DefinesStreamProxy proxy(&mockStream);

        ExpectLine(&proxy, 1, "");
        ExpectLine(&proxy, 2, "");
        ExpectLine(&proxy, 3, "\"HelloWorld\"");
        ExpectLine(&proxy, 4, "\"Hello5\"");

        REQUIRE(proxy.Eof());
    }
} // namespace test::parsing::impl::defines_stream_proxy
