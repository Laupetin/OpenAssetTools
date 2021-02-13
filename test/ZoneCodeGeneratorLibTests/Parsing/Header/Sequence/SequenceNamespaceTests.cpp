#include <catch2/catch.hpp>

#include "Parsing/Header/Sequence/SequenceNamespace.h"
#include "Parsing/Mock/MockLexer.h"

namespace test::parsing::header::sequence::sequence_namespace
{
    TEST_CASE("SequenceNamespace: Ensure can parse simple namespace directive", "[parsing][parsingstream]")
    {
        const TokenPos pos;
        const auto lexer = std::make_unique<MockLexer<HeaderParserValue>>(MockLexer<HeaderParserValue>(
            {
                HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
                HeaderParserValue::Identifier(pos, new std::string("test_namespace")),
                HeaderParserValue::Character(pos, '{')
            }, HeaderParserValue::EndOfFile(pos)));

        const auto sequence = std::make_unique<SequenceNamespace>();
        const auto state = std::make_unique<HeaderParserState>();

        unsigned consumedTokenCount;
        auto result = sequence->MatchSequence(lexer.get(), state.get(), consumedTokenCount);

        REQUIRE(result);
    }
}
