#include "Parsing/Commands/Sequence/SequencePointerArrayWordSize.h"
#include "Parsing/Commands/Sequence/SequenceReusablePointerArrayElements.h"
#include "Parsing/Commands/Sequence/SequenceSetAliasWordSize.h"
#include "Parsing/Commands/Sequence/SequenceSetWordSize.h"
#include "Parsing/Mock/MockLexer.h"
#include "Parsing/PostProcessing/CreateMemberInformationPostProcessor.h"
#include "Parsing/PostProcessing/CreateStructureInformationPostProcessor.h"
#include "Persistence/InMemory/InMemoryRepository.h"

#include <catch2/catch_test_macros.hpp>

namespace test::parsing::commands::sequence::sequence_serialized_abi
{
    class CommandsSequenceTestsHelper
    {
    public:
        std::unique_ptr<IDataRepository> m_repository;
        std::unique_ptr<CommandsParserState> m_state;
        std::unique_ptr<ILexer<CommandsParserValue>> m_lexer;
        StructureInformation* m_test_struct;
        MemberInformation* m_test_member;
        unsigned m_consumed_token_count;

        CommandsSequenceTestsHelper()
            : m_repository(std::make_unique<InMemoryRepository>()),
              m_state(std::make_unique<CommandsParserState>(m_repository.get())),
              m_test_struct(nullptr),
              m_test_member(nullptr),
              m_consumed_token_count(0u)
        {
            auto definition = std::make_unique<StructDefinition>("", "test_struct_t", 8);
            definition->m_members.emplace_back(std::make_shared<Variable>("m_items", std::make_unique<TypeDeclaration>(BaseTypeDefinition::INT)));
            const auto* definitionPtr = definition.get();
            m_repository->Add(std::move(definition));

            REQUIRE(CreateStructureInformationPostProcessor().PostProcess(m_repository.get()));
            REQUIRE(CreateMemberInformationPostProcessor().PostProcess(m_repository.get()));

            m_test_struct = m_repository->GetInformationFor(definitionPtr);
            REQUIRE(m_test_struct != nullptr);
            REQUIRE(m_test_struct->m_ordered_members.size() == 1);
            m_test_member = m_test_struct->m_ordered_members[0].get();
            REQUIRE(m_test_member != nullptr);
        }

        void Tokens(std::initializer_list<Movable<CommandsParserValue>> tokens)
        {
            m_lexer = std::make_unique<MockLexer<CommandsParserValue>>(tokens, CommandsParserValue::EndOfFile(TokenPos()));
        }

        template<typename TSequence> bool PerformTest()
        {
            REQUIRE(m_lexer);
            const auto sequence = std::make_unique<TSequence>();
            return sequence->MatchSequence(m_lexer.get(), m_state.get(), m_consumed_token_count);
        }
    };

    TEST_CASE("SequenceSetWordSize: Sets the serialized structure word size", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.m_state->SetInUse(helper.m_test_struct);
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("wordsize")),
            CommandsParserValue::Integer(pos, 32),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        REQUIRE(helper.PerformTest<SequenceSetWordSize>());
        REQUIRE(helper.m_consumed_token_count == 4);
        REQUIRE(helper.m_test_struct->m_word_size == WordSize::BITS_32);
    }

    TEST_CASE("SequenceSetAliasWordSize: Sets the serialized alias word size", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.m_state->SetInUse(helper.m_test_struct);
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("aliaswordsize")),
            CommandsParserValue::Integer(pos, 32),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        REQUIRE(helper.PerformTest<SequenceSetAliasWordSize>());
        REQUIRE(helper.m_consumed_token_count == 4);
        REQUIRE(helper.m_test_struct->m_alias_word_size == WordSize::BITS_32);
    }

    TEST_CASE("SequenceSetAliasWordSize: Requires an active structure", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("aliaswordsize")),
            CommandsParserValue::Integer(pos, 32),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        REQUIRE_THROWS_AS(helper.PerformTest<SequenceSetAliasWordSize>(), ParsingException);
        REQUIRE(helper.m_test_struct->m_alias_word_size == WordSize::UNKNOWN);
    }

    TEST_CASE("SequencePointerArrayWordSize: Sets the serialized pointer-array word size", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("pointerarraywordsize")),
            CommandsParserValue::Identifier(pos, new std::string("test_struct_t")),
            CommandsParserValue::Character(pos, ':'),
            CommandsParserValue::Character(pos, ':'),
            CommandsParserValue::Identifier(pos, new std::string("m_items")),
            CommandsParserValue::Integer(pos, 32),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        REQUIRE(helper.PerformTest<SequencePointerArrayWordSize>());
        REQUIRE(helper.m_consumed_token_count == 8);
        REQUIRE(helper.m_test_member->m_pointer_array_word_size == WordSize::BITS_32);
    }

    TEST_CASE("SequenceReusablePointerArrayElements: Marks pointer-array elements reusable", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("reusable")),
            CommandsParserValue::Identifier(pos, new std::string("elements")),
            CommandsParserValue::Identifier(pos, new std::string("test_struct_t")),
            CommandsParserValue::Character(pos, ':'),
            CommandsParserValue::Character(pos, ':'),
            CommandsParserValue::Identifier(pos, new std::string("m_items")),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        REQUIRE(helper.PerformTest<SequenceReusablePointerArrayElements>());
        REQUIRE(helper.m_consumed_token_count == 8);
        REQUIRE(helper.m_test_member->m_pointer_array_elements_are_reusable);
    }
} // namespace test::parsing::commands::sequence::sequence_serialized_abi
