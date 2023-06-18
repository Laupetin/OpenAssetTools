#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "Utils/ClassUtils.h"
#include "Parsing/Commands/Sequence/SequenceGame.h"
#include "Parsing/Mock/MockLexer.h"
#include "Persistence/InMemory/InMemoryRepository.h"

namespace test::parsing::commands::sequence::sequence_game
{
    class CommandsSequenceTestsHelper
    {
    public:
        std::unique_ptr<IDataRepository> m_repository;
        std::unique_ptr<CommandsParserState> m_state;
        std::unique_ptr<ILexer<CommandsParserValue>> m_lexer;

        unsigned m_consumed_token_count;

        CommandsSequenceTestsHelper()
            : m_repository(std::make_unique<InMemoryRepository>()),
              m_state(std::make_unique<CommandsParserState>(m_repository.get())),
              m_consumed_token_count(0u)
        {
        }

        void Tokens(std::initializer_list<Movable<CommandsParserValue>> tokens)
        {
            m_lexer = std::make_unique<MockLexer<CommandsParserValue>>(tokens, CommandsParserValue::EndOfFile(TokenPos()));
        }

        bool PerformTest()
        {
            REQUIRE(m_lexer);
            const auto sequence = std::make_unique<SequenceGame>();
            return sequence->MatchSequence(m_lexer.get(), m_state.get(), m_consumed_token_count);
        }
    };

    TEST_CASE("SequenceGame: Ensure can set game", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("game")),
            CommandsParserValue::Identifier(pos, new std::string("very_cool_game")),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos)
        });

        auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);
        REQUIRE(helper.m_repository->GetGameName() == "very_cool_game");
    }
}
