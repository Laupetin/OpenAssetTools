#include <catch2/catch.hpp>

#include "Parsing/Header/Impl/HeaderLexer.h"
#include "Parsing/Mock/MockParserLineStream.h"

namespace test::parsing::header::impl::header_lexer
{
    void ExpectCharacterToken(HeaderLexer& lexer, char c)
    {
        REQUIRE(lexer.GetToken(0).m_type == HeaderParserValueType::CHARACTER);
        REQUIRE(lexer.GetToken(0).CharacterValue() == c);
        lexer.PopTokens(1);
    }

    void ExpectIntegerToken(HeaderLexer& lexer, int number)
    {
        REQUIRE(lexer.GetToken(0).m_type == HeaderParserValueType::INTEGER);
        REQUIRE(lexer.GetToken(0).IntegerValue() == number);
        lexer.PopTokens(1);
    }

    void ExpectFloatingPointToken(HeaderLexer& lexer, double number)
    {
        REQUIRE(lexer.GetToken(0).m_type == HeaderParserValueType::FLOATING_POINT);
        REQUIRE(lexer.GetToken(0).FloatingPointValue() == Approx(number));
        lexer.PopTokens(1);
    }

    void ExpectIdentifierToken(HeaderLexer& lexer, const std::string& identifier)
    {
        REQUIRE(lexer.GetToken(0).m_type == HeaderParserValueType::IDENTIFIER);
        REQUIRE(lexer.GetToken(0).IdentifierValue() == identifier);
        lexer.PopTokens(1);
    }

    void ExpectStringToken(HeaderLexer& lexer, const std::string& c)
    {
        REQUIRE(lexer.GetToken(0).m_type == HeaderParserValueType::STRING);
        REQUIRE(lexer.GetToken(0).StringValue() == c);
        lexer.PopTokens(1);
    }

    void ExpectTokenWithType(HeaderLexer& lexer, HeaderParserValueType type)
    {
        REQUIRE(lexer.GetToken(0).m_type == type);
        lexer.PopTokens(1);
    }

    TEST_CASE("HeaderLexer: Ensure can parse simple hex numbers", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "0x1A4",
            " 0xABC ",
            " 0xAAA",
            "0xBBB "
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectIntegerToken(lexer, 0x1a4);
        ExpectIntegerToken(lexer, 0xabc);
        ExpectIntegerToken(lexer, 0xaaa);
        ExpectIntegerToken(lexer, 0xbbb);

        REQUIRE(lexer.GetToken(0).m_type == HeaderParserValueType::END_OF_FILE);
    }

    TEST_CASE("HeaderLexer: Ensure can parse simple hex numbers surrounded by identifiers", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "abc 0xABC cba",
            "aaa 0xAAA",
            "0xBBB bbb"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectIdentifierToken(lexer, "abc");
        ExpectIntegerToken(lexer, 0xabc);
        ExpectIdentifierToken(lexer, "cba");

        ExpectIdentifierToken(lexer, "aaa");
        ExpectIntegerToken(lexer, 0xaaa);

        ExpectIntegerToken(lexer, 0xbbb);
        ExpectIdentifierToken(lexer, "bbb");

        REQUIRE(lexer.GetToken(0).m_type == HeaderParserValueType::END_OF_FILE);
    }

    TEST_CASE("HeaderLexer: Ensure throws exception when parsing incomplete hex number", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "0x"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        REQUIRE_THROWS_AS(lexer.GetToken(0), ParsingException);
    }

    TEST_CASE("HeaderLexer: Ensure throws exception when parsing invalid hex number", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "0xGEGE"
        };
        
        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        REQUIRE_THROWS_AS(lexer.GetToken(0), ParsingException);
    }

    TEST_CASE("HeaderLexer: Ensure throws exception when parsing invalid hex number that starts with a valid letter", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "0xEGEG"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        REQUIRE_THROWS_AS(lexer.GetToken(0), ParsingException);
    }

    TEST_CASE("HeaderLexer: Ensure can parse simple integers", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "  524   ",
            "4221111 1337  ",
            "0 420"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectIntegerToken(lexer, 524);
        ExpectIntegerToken(lexer, 4221111);
        ExpectIntegerToken(lexer, 1337);
        ExpectIntegerToken(lexer, 0);
        ExpectIntegerToken(lexer, 420);
    }

    TEST_CASE("HeaderLexer: Ensure can parse integers surrounded by identifiers", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "aa 6 bb",
            "123456789 ccc",
            "0 d 420"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectIdentifierToken(lexer, "aa");
        ExpectIntegerToken(lexer, 6);
        ExpectIdentifierToken(lexer, "bb");
        ExpectIntegerToken(lexer, 123456789);
        ExpectIdentifierToken(lexer, "ccc");
        ExpectIntegerToken(lexer, 0);
        ExpectIdentifierToken(lexer, "d");
        ExpectIntegerToken(lexer, 420);
    }

    TEST_CASE("HeaderLexer: Ensure parses negative numbers as character and number", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "-1337"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);
        
        ExpectCharacterToken(lexer, '-');
        ExpectIntegerToken(lexer, 1337);
    }

    TEST_CASE("HeaderLexer: Ensure recognizes numbers surrounded by characters", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "(1337)"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);
        
        ExpectCharacterToken(lexer, '(');
        ExpectIntegerToken(lexer, 1337);
        ExpectCharacterToken(lexer, ')');
    }

    TEST_CASE("HeaderLexer: Ensure parses simple floating point numbers", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "420.1337"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);
        
        ExpectFloatingPointToken(lexer, 420.1337);
    }

    TEST_CASE("HeaderLexer: Ensure parses simple floating point numbers surrounded by identifiers", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "aa 50.24 a",
            "b 36.999",
            "59595.2414 c"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectIdentifierToken(lexer, "aa");
        ExpectFloatingPointToken(lexer, 50.24);
        ExpectIdentifierToken(lexer, "a");
        ExpectIdentifierToken(lexer, "b");
        ExpectFloatingPointToken(lexer, 36.999);
        ExpectFloatingPointToken(lexer, 59595.2412);
        ExpectIdentifierToken(lexer, "c");
    }

    TEST_CASE("HeaderLexer: Ensure recognizes floating point numbers surrounded by characters", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "(1337.420)"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectCharacterToken(lexer, '(');
        ExpectFloatingPointToken(lexer, 1337.420);
        ExpectCharacterToken(lexer, ')');
    }

    TEST_CASE("HeaderLexer: Ensure can separate identifiers with symbols", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "hello|world hello_+universe"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectIdentifierToken(lexer, "hello");
        ExpectCharacterToken(lexer, '|');
        ExpectIdentifierToken(lexer, "world");
        ExpectIdentifierToken(lexer, "hello_");
        ExpectCharacterToken(lexer, '+');
        ExpectIdentifierToken(lexer, "universe");
    }

    TEST_CASE("HeaderLexer: Can recognize shift left", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "<<hello<<world<<"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectTokenWithType(lexer, HeaderParserValueType::SHIFT_LEFT);
        ExpectIdentifierToken(lexer, "hello");
        ExpectTokenWithType(lexer, HeaderParserValueType::SHIFT_LEFT);
        ExpectIdentifierToken(lexer, "world");
        ExpectTokenWithType(lexer, HeaderParserValueType::SHIFT_LEFT);
    }

    TEST_CASE("HeaderLexer: Can recognize less equals", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "<=hello<=world<="
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectTokenWithType(lexer, HeaderParserValueType::LESS_EQUAL);
        ExpectIdentifierToken(lexer, "hello");
        ExpectTokenWithType(lexer, HeaderParserValueType::LESS_EQUAL);
        ExpectIdentifierToken(lexer, "world");
        ExpectTokenWithType(lexer, HeaderParserValueType::LESS_EQUAL);
    }

    TEST_CASE("HeaderLexer: Can recognize less", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "<%hello<world<&"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectCharacterToken(lexer, '<');
        ExpectCharacterToken(lexer, '%');
        ExpectIdentifierToken(lexer, "hello");
        ExpectCharacterToken(lexer, '<');
        ExpectIdentifierToken(lexer, "world");
        ExpectCharacterToken(lexer, '<');
        ExpectCharacterToken(lexer, '&');
    }

    TEST_CASE("HeaderLexer: Can recognize shift right", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            ">>hello>>world>>"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectTokenWithType(lexer, HeaderParserValueType::SHIFT_RIGHT);
        ExpectIdentifierToken(lexer, "hello");
        ExpectTokenWithType(lexer, HeaderParserValueType::SHIFT_RIGHT);
        ExpectIdentifierToken(lexer, "world");
        ExpectTokenWithType(lexer, HeaderParserValueType::SHIFT_RIGHT);
    }

    TEST_CASE("HeaderLexer: Can recognize greater equals", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            ">=hello>=world>="
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectTokenWithType(lexer, HeaderParserValueType::GREATER_EQUAL);
        ExpectIdentifierToken(lexer, "hello");
        ExpectTokenWithType(lexer, HeaderParserValueType::GREATER_EQUAL);
        ExpectIdentifierToken(lexer, "world");
        ExpectTokenWithType(lexer, HeaderParserValueType::GREATER_EQUAL);
    }

    TEST_CASE("HeaderLexer: Can recognize greater", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            ">%hello>world>&"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectCharacterToken(lexer, '>');
        ExpectCharacterToken(lexer, '%');
        ExpectIdentifierToken(lexer, "hello");
        ExpectCharacterToken(lexer, '>');
        ExpectIdentifierToken(lexer, "world");
        ExpectCharacterToken(lexer, '>');
        ExpectCharacterToken(lexer, '&');
    }

    TEST_CASE("HeaderLexer: Can recognize equals", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "==hello==world=="
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectTokenWithType(lexer, HeaderParserValueType::EQUALS);
        ExpectIdentifierToken(lexer, "hello");
        ExpectTokenWithType(lexer, HeaderParserValueType::EQUALS);
        ExpectIdentifierToken(lexer, "world");
        ExpectTokenWithType(lexer, HeaderParserValueType::EQUALS);
    }

    TEST_CASE("HeaderLexer: Can recognize assign", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "=%hello=world=&"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectCharacterToken(lexer, '=');
        ExpectCharacterToken(lexer, '%');
        ExpectIdentifierToken(lexer, "hello");
        ExpectCharacterToken(lexer, '=');
        ExpectIdentifierToken(lexer, "world");
        ExpectCharacterToken(lexer, '=');
        ExpectCharacterToken(lexer, '&');
    }

    TEST_CASE("HeaderLexer: Can recognize logical and", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "&&hello&&world&&"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectTokenWithType(lexer, HeaderParserValueType::LOGICAL_AND);
        ExpectIdentifierToken(lexer, "hello");
        ExpectTokenWithType(lexer, HeaderParserValueType::LOGICAL_AND);
        ExpectIdentifierToken(lexer, "world");
        ExpectTokenWithType(lexer, HeaderParserValueType::LOGICAL_AND);
    }

    TEST_CASE("HeaderLexer: Can recognize ampersand", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "&%hello&world&+"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectCharacterToken(lexer, '&');
        ExpectCharacterToken(lexer, '%');
        ExpectIdentifierToken(lexer, "hello");
        ExpectCharacterToken(lexer, '&');
        ExpectIdentifierToken(lexer, "world");
        ExpectCharacterToken(lexer, '&');
        ExpectCharacterToken(lexer, '+');
    }

    TEST_CASE("HeaderLexer: Can recognize logical or", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "||hello||world||"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectTokenWithType(lexer, HeaderParserValueType::LOGICAL_OR);
        ExpectIdentifierToken(lexer, "hello");
        ExpectTokenWithType(lexer, HeaderParserValueType::LOGICAL_OR);
        ExpectIdentifierToken(lexer, "world");
        ExpectTokenWithType(lexer, HeaderParserValueType::LOGICAL_OR);
    }

    TEST_CASE("HeaderLexer: Can recognize pipe", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "|%hello|world|&"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectCharacterToken(lexer, '|');
        ExpectCharacterToken(lexer, '%');
        ExpectIdentifierToken(lexer, "hello");
        ExpectCharacterToken(lexer, '|');
        ExpectIdentifierToken(lexer, "world");
        ExpectCharacterToken(lexer, '|');
        ExpectCharacterToken(lexer, '&');
    }

    TEST_CASE("HeaderLexer: Can recognize not equals", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "!=hello!=world!="
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectTokenWithType(lexer, HeaderParserValueType::NOT_EQUAL);
        ExpectIdentifierToken(lexer, "hello");
        ExpectTokenWithType(lexer, HeaderParserValueType::NOT_EQUAL);
        ExpectIdentifierToken(lexer, "world");
        ExpectTokenWithType(lexer, HeaderParserValueType::NOT_EQUAL);
    }

    TEST_CASE("HeaderLexer: Can recognize exclamation mark", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "!%hello!world!&"
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectCharacterToken(lexer, '!');
        ExpectCharacterToken(lexer, '%');
        ExpectIdentifierToken(lexer, "hello");
        ExpectCharacterToken(lexer, '!');
        ExpectIdentifierToken(lexer, "world");
        ExpectCharacterToken(lexer, '!');
        ExpectCharacterToken(lexer, '&');
    }

    TEST_CASE("HeaderLexer: Can recognize strings", "[parsing][header]")
    {
        const std::vector<std::string> lines
        {
            "\"hello world\"",
            "a\"hi there\"bbb",
            " \"nice\"",
            "\"meme\" "
        };

        MockParserLineStream mockStream(lines);
        HeaderLexer lexer(&mockStream);

        ExpectStringToken(lexer, "hello world");
        ExpectIdentifierToken(lexer, "a");
        ExpectStringToken(lexer, "hi there");
        ExpectIdentifierToken(lexer, "bbb");
        ExpectStringToken(lexer, "nice");
        ExpectStringToken(lexer, "meme");
    }
}
