#include "Parsing/Header/Sequence/SequenceNamespace.h"
#include "Parsing/Mock/MockLexer.h"
#include "Parsing/Mock/MockPackValueSupplier.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace test::parsing::header::sequence::sequence_namespace
{
    TEST_CASE("SequenceNamespace: Ensure can parse simple namespace directive", "[parsing][parsingstream]")
    {
        const TokenPos pos;
        const auto lexer =
            std::make_unique<MockLexer<HeaderParserValue>>(MockLexer<HeaderParserValue>({HeaderParserValue::Keyword(pos, HeaderParserValueType::NAMESPACE),
                                                                                         HeaderParserValue::Identifier(pos, new std::string("test_namespace")),
                                                                                         HeaderParserValue::Character(pos, '{')},
                                                                                        HeaderParserValue::EndOfFile(pos)));

        const auto packValueSupplier = std::make_unique<MockPackValueSupplier>();
        const auto sequence = std::make_unique<SequenceNamespace>();
        const auto state = std::make_unique<HeaderParserState>(packValueSupplier.get());

        unsigned consumedTokenCount;
        auto result = sequence->MatchSequence(lexer.get(), state.get(), consumedTokenCount);

        REQUIRE(result);
    }
} // namespace test::parsing::header::sequence::sequence_namespace
