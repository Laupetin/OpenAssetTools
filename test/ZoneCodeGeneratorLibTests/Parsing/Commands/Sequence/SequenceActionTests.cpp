#include "Parsing/Commands/Sequence/SequenceAction.h"
#include "Parsing/Mock/MockLexer.h"
#include "Parsing/PostProcessing/CreateMemberInformationPostProcessor.h"
#include "Parsing/PostProcessing/CreateStructureInformationPostProcessor.h"
#include "Persistence/InMemory/InMemoryRepository.h"
#include "Utils/ClassUtils.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace test::parsing::commands::sequence::sequence_action
{
    class CommandsSequenceTestsHelper
    {
    public:
        std::unique_ptr<IDataRepository> m_repository;
        std::unique_ptr<CommandsParserState> m_state;
        std::unique_ptr<ILexer<CommandsParserValue>> m_lexer;

        StructDefinition* m_test_struct_t;
        StructureInformation* m_test_struct;

        StructDefinition* m_container_struct_t;
        StructureInformation* m_container_struct;
        MemberInformation* m_container_struct_child;

        StructDefinition* m_arg_struct_raw;
        StructureInformation* m_arg_struct;

        StructDefinition* m_arg_struct2_raw;
        StructureInformation* m_arg_struct2;

        StructDefinition* m_arg_struct3_raw;
        StructureInformation* m_arg_struct3;

        unsigned m_consumed_token_count;

    private:
        void RetrieveInformationPointers()
        {
            m_test_struct = m_repository->GetInformationFor(m_test_struct_t);
            REQUIRE(m_test_struct != nullptr);
            m_container_struct = m_repository->GetInformationFor(m_container_struct_t);
            REQUIRE(m_container_struct != nullptr);
            m_container_struct_child = m_container_struct->m_ordered_members[0].get();
            REQUIRE(m_container_struct_child != nullptr);
            m_arg_struct = m_repository->GetInformationFor(m_arg_struct_raw);
            REQUIRE(m_arg_struct != nullptr);
            m_arg_struct2 = m_repository->GetInformationFor(m_arg_struct2_raw);
            REQUIRE(m_arg_struct2 != nullptr);
            m_arg_struct3 = m_repository->GetInformationFor(m_arg_struct3_raw);
            REQUIRE(m_arg_struct3 != nullptr);
        }

        _NODISCARD bool CreateInformation() const
        {
            auto createStructureInformation = std::make_unique<CreateStructureInformationPostProcessor>();
            auto createMemberInformation = std::make_unique<CreateMemberInformationPostProcessor>();

            return createStructureInformation->PostProcess(m_repository.get()) && createMemberInformation->PostProcess(m_repository.get());
        }

        void AddSampleData()
        {
            auto def = std::make_unique<StructDefinition>("", "test_struct_t", 8);
            def->m_members.emplace_back(std::make_shared<Variable>("m_test", std::make_unique<TypeDeclaration>(BaseTypeDefinition::BOOL)));
            m_test_struct_t = def.get();
            m_repository->Add(std::move(def));

            def = std::make_unique<StructDefinition>("", "container_struct_t", 8);
            def->m_members.emplace_back(std::make_shared<Variable>("m_child", std::make_unique<TypeDeclaration>(m_test_struct_t)));
            m_container_struct_t = def.get();
            m_repository->Add(std::move(def));

            def = std::make_unique<StructDefinition>("", "arg_t", 8);
            def->m_members.emplace_back(std::make_shared<Variable>("m_hello_world", std::make_unique<TypeDeclaration>(BaseTypeDefinition::INT)));
            m_arg_struct_raw = def.get();
            m_repository->Add(std::move(def));

            def = std::make_unique<StructDefinition>("", "another_arg_t", 8);
            def->m_members.emplace_back(std::make_shared<Variable>("m_hello_galaxy", std::make_unique<TypeDeclaration>(BaseTypeDefinition::DOUBLE)));
            def->m_members.emplace_back(std::make_shared<Variable>("m_hello_universe", std::make_unique<TypeDeclaration>(BaseTypeDefinition::CHAR)));
            m_arg_struct2_raw = def.get();
            m_repository->Add(std::move(def));

            def = std::make_unique<StructDefinition>("", "yeet_t", 8);
            def->m_members.emplace_back(std::make_shared<Variable>("m_ya", std::make_unique<TypeDeclaration>(m_arg_struct_raw)));
            def->m_members.emplace_back(std::make_shared<Variable>("m_yeeeeeet", std::make_unique<TypeDeclaration>(m_arg_struct2_raw)));
            m_arg_struct3_raw = def.get();
            m_repository->Add(std::move(def));
        }

    public:
        CommandsSequenceTestsHelper()
            : m_repository(std::make_unique<InMemoryRepository>()),
              m_state(std::make_unique<CommandsParserState>(m_repository.get())),
              m_test_struct_t(nullptr),
              m_test_struct(nullptr),
              m_container_struct_t(nullptr),
              m_container_struct(nullptr),
              m_arg_struct_raw(nullptr),
              m_arg_struct(nullptr),
              m_arg_struct2_raw(nullptr),
              m_arg_struct2(nullptr),
              m_arg_struct3_raw(nullptr),
              m_arg_struct3(nullptr),
              m_consumed_token_count(0u)
        {
            AddSampleData();
            REQUIRE(CreateInformation());
            RetrieveInformationPointers();
        }

        void Tokens(std::initializer_list<Movable<CommandsParserValue>> tokens)
        {
            m_lexer = std::make_unique<MockLexer<CommandsParserValue>>(tokens, CommandsParserValue::EndOfFile(TokenPos()));
        }

        bool PerformTest()
        {
            REQUIRE(m_lexer);
            const auto sequence = std::make_unique<SequenceAction>();
            return sequence->MatchSequence(m_lexer.get(), m_state.get(), m_consumed_token_count);
        }
    };

    TEST_CASE("SequenceAction: Ensure can parse simple action directive", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("action")),
            CommandsParserValue::Identifier(pos, new std::string("test_struct_t")),
            CommandsParserValue::Identifier(pos, new std::string("TestMethod")),
            CommandsParserValue::Character(pos, '('),
            CommandsParserValue::Character(pos, ')'),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 7);
        REQUIRE(helper.m_test_struct->m_post_load_action);
        REQUIRE(helper.m_test_struct->m_post_load_action->m_action_name == "TestMethod");
        REQUIRE(helper.m_test_struct->m_post_load_action->m_parameter_types.empty());
    }

    TEST_CASE("SequenceAction: Ensure can parse simple action directive with one arg", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("action")),
            CommandsParserValue::Identifier(pos, new std::string("test_struct_t")),
            CommandsParserValue::Identifier(pos, new std::string("TestMethod")),
            CommandsParserValue::Character(pos, '('),
            CommandsParserValue::Identifier(pos, new std::string("arg_t")),
            CommandsParserValue::Character(pos, ')'),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 8);
        REQUIRE(helper.m_test_struct->m_post_load_action);
        REQUIRE(helper.m_test_struct->m_post_load_action->m_action_name == "TestMethod");
        REQUIRE(helper.m_test_struct->m_post_load_action->m_parameter_types.size() == 1);
        REQUIRE(helper.m_test_struct->m_post_load_action->m_parameter_types[0] == helper.m_arg_struct_raw);
    }

    TEST_CASE("SequenceAction: Ensure can parse simple action directive with two args", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("action")),
            CommandsParserValue::Identifier(pos, new std::string("test_struct_t")),
            CommandsParserValue::Identifier(pos, new std::string("TestMethod")),
            CommandsParserValue::Character(pos, '('),
            CommandsParserValue::Identifier(pos, new std::string("arg_t")),
            CommandsParserValue::Character(pos, ','),
            CommandsParserValue::Identifier(pos, new std::string("another_arg_t")),
            CommandsParserValue::Character(pos, ')'),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 10);
        REQUIRE(helper.m_test_struct->m_post_load_action);
        REQUIRE(helper.m_test_struct->m_post_load_action->m_action_name == "TestMethod");
        REQUIRE(helper.m_test_struct->m_post_load_action->m_parameter_types.size() == 2);
        REQUIRE(helper.m_test_struct->m_post_load_action->m_parameter_types[0] == helper.m_arg_struct_raw);
        REQUIRE(helper.m_test_struct->m_post_load_action->m_parameter_types[1] == helper.m_arg_struct2_raw);
    }

    TEST_CASE("SequenceAction: Ensure can parse simple action directive with three args", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("action")),
            CommandsParserValue::Identifier(pos, new std::string("test_struct_t")),
            CommandsParserValue::Identifier(pos, new std::string("TestMethod")),
            CommandsParserValue::Character(pos, '('),
            CommandsParserValue::Identifier(pos, new std::string("arg_t")),
            CommandsParserValue::Character(pos, ','),
            CommandsParserValue::Identifier(pos, new std::string("another_arg_t")),
            CommandsParserValue::Character(pos, ','),
            CommandsParserValue::Identifier(pos, new std::string("yeet_t")),
            CommandsParserValue::Character(pos, ')'),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 12);
        REQUIRE(helper.m_test_struct->m_post_load_action);
        REQUIRE(helper.m_test_struct->m_post_load_action->m_action_name == "TestMethod");
        REQUIRE(helper.m_test_struct->m_post_load_action->m_parameter_types.size() == 3);
        REQUIRE(helper.m_test_struct->m_post_load_action->m_parameter_types[0] == helper.m_arg_struct_raw);
        REQUIRE(helper.m_test_struct->m_post_load_action->m_parameter_types[1] == helper.m_arg_struct2_raw);
        REQUIRE(helper.m_test_struct->m_post_load_action->m_parameter_types[2] == helper.m_arg_struct3_raw);
    }

    TEST_CASE("SequenceAction: Fails if base typename does not exist", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("action")),
            CommandsParserValue::Identifier(pos, new std::string("unknown_struct_t")),
            CommandsParserValue::Identifier(pos, new std::string("TestMethod")),
            CommandsParserValue::Character(pos, '('),
            CommandsParserValue::Character(pos, ')'),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        REQUIRE_THROWS_AS(helper.PerformTest(), ParsingException);
        REQUIRE(helper.m_test_struct->m_post_load_action == nullptr);
    }

    TEST_CASE("SequenceAction: Fails if arg typename does not exist", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("action")),
            CommandsParserValue::Identifier(pos, new std::string("test_struct_t")),
            CommandsParserValue::Identifier(pos, new std::string("TestMethod")),
            CommandsParserValue::Character(pos, '('),
            CommandsParserValue::Identifier(pos, new std::string("arg_t")),
            CommandsParserValue::Character(pos, ','),
            CommandsParserValue::Identifier(pos, new std::string("another_arg_t")),
            CommandsParserValue::Character(pos, ','),
            CommandsParserValue::Identifier(pos, new std::string("unknown_type_t")),
            CommandsParserValue::Character(pos, ')'),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        REQUIRE_THROWS_AS(helper.PerformTest(), ParsingException);
        REQUIRE(helper.m_test_struct->m_post_load_action == nullptr);
    }

    TEST_CASE("SequenceAction: Fails if no type and no use", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("action")),
            CommandsParserValue::Identifier(pos, new std::string("TestMethod")),
            CommandsParserValue::Character(pos, '('),
            CommandsParserValue::Character(pos, ')'),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        REQUIRE_THROWS_AS(helper.PerformTest(), ParsingException);
        REQUIRE(helper.m_test_struct->m_post_load_action == nullptr);
    }

    TEST_CASE("SequenceAction: Ensure can parse action directive with used type", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("action")),
            CommandsParserValue::Identifier(pos, new std::string("TestMethod")),
            CommandsParserValue::Character(pos, '('),
            CommandsParserValue::Character(pos, ')'),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });
        helper.m_state->SetInUse(helper.m_test_struct);

        auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 6);
        REQUIRE(helper.m_test_struct->m_post_load_action);
        REQUIRE(helper.m_test_struct->m_post_load_action->m_action_name == "TestMethod");
        REQUIRE(helper.m_test_struct->m_post_load_action->m_parameter_types.empty());
    }

    TEST_CASE("SequenceAction: Ensure can parse action directive with type from member", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("action")),
            CommandsParserValue::Identifier(pos, new std::string("container_struct_t")),
            CommandsParserValue::Character(pos, ':'),
            CommandsParserValue::Character(pos, ':'),
            CommandsParserValue::Identifier(pos, new std::string("m_child")),
            CommandsParserValue::Identifier(pos, new std::string("TestMethod")),
            CommandsParserValue::Character(pos, '('),
            CommandsParserValue::Character(pos, ')'),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });

        auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 10);
        REQUIRE(helper.m_test_struct->m_post_load_action == nullptr);

        REQUIRE(helper.m_container_struct_child->m_post_load_action);
        REQUIRE(helper.m_container_struct_child->m_post_load_action->m_action_name == "TestMethod");
        REQUIRE(helper.m_container_struct_child->m_post_load_action->m_parameter_types.empty());
    }

    TEST_CASE("SequenceAction: Ensure can parse action directive with type from member and in use", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("action")),
            CommandsParserValue::Identifier(pos, new std::string("m_child")),
            CommandsParserValue::Identifier(pos, new std::string("TestMethod")),
            CommandsParserValue::Character(pos, '('),
            CommandsParserValue::Character(pos, ')'),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });
        helper.m_state->SetInUse(helper.m_container_struct);

        auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 7);
        REQUIRE(helper.m_test_struct->m_post_load_action == nullptr);

        REQUIRE(helper.m_container_struct_child->m_post_load_action);
        REQUIRE(helper.m_container_struct_child->m_post_load_action->m_action_name == "TestMethod");
        REQUIRE(helper.m_container_struct_child->m_post_load_action->m_parameter_types.empty());
    }

    TEST_CASE("SequenceAction: Ensure can use different struct even though something is used", "[parsing][sequence]")
    {
        CommandsSequenceTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            CommandsParserValue::Identifier(pos, new std::string("set")),
            CommandsParserValue::Identifier(pos, new std::string("action")),
            CommandsParserValue::Identifier(pos, new std::string("container_struct_t")),
            CommandsParserValue::Character(pos, ':'),
            CommandsParserValue::Character(pos, ':'),
            CommandsParserValue::Identifier(pos, new std::string("m_child")),
            CommandsParserValue::Identifier(pos, new std::string("TestMethod")),
            CommandsParserValue::Character(pos, '('),
            CommandsParserValue::Character(pos, ')'),
            CommandsParserValue::Character(pos, ';'),
            CommandsParserValue::EndOfFile(pos),
        });
        helper.m_state->SetInUse(helper.m_arg_struct2);

        auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 10);
        REQUIRE(helper.m_test_struct->m_post_load_action == nullptr);

        REQUIRE(helper.m_container_struct_child->m_post_load_action);
        REQUIRE(helper.m_container_struct_child->m_post_load_action->m_action_name == "TestMethod");
        REQUIRE(helper.m_container_struct_child->m_post_load_action->m_parameter_types.empty());
    }
} // namespace test::parsing::commands::sequence::sequence_action
