#include <catch2/catch.hpp>

#include "Parsing/Menu/Domain/EventHandler/CommonEventHandlerScript.h"
#include "Utils/ClassUtils.h"
#include "Parsing/Menu/Sequence/EventHandlerSetScopeSequences.h"
#include "Parsing/Mock/MockLexer.h"

using namespace menu;

namespace test::parsing::menu::sequence::event_handler_set
{
    class EventHandlerSetSequenceTestsHelper
    {
    public:
        std::vector<std::unique_ptr<MenuFileParser::sequence_t>> m_all_sequences;
        std::vector<MenuFileParser::sequence_t*> m_scope_sequences;
        std::unique_ptr<MenuFileParserState> m_state;
        std::unique_ptr<ILexer<SimpleParserValue>> m_lexer;

        std::unique_ptr<CommonEventHandlerSet> m_event_handler_set;

        unsigned m_consumed_token_count;

        explicit EventHandlerSetSequenceTestsHelper(FeatureLevel featureLevel)
            : m_state(std::make_unique<MenuFileParserState>(featureLevel)),
              m_event_handler_set(std::make_unique<CommonEventHandlerSet>()),
              m_consumed_token_count(0u)
        {
            EventHandlerSetScopeSequences scopeSequences(m_all_sequences, m_scope_sequences);
            scopeSequences.AddSequences(m_state->m_feature_level);

            m_state->m_current_menu = m_state->m_menus.emplace_back(std::make_unique<CommonMenuDef>()).get();
            m_state->m_current_event_handler_set = m_event_handler_set.get();
            m_state->m_current_nested_event_handler_set = m_event_handler_set.get();
        }

        void Tokens(std::initializer_list<Movable<SimpleParserValue>> tokens)
        {
            m_lexer = std::make_unique<MockLexer<SimpleParserValue>>(tokens, SimpleParserValue::EndOfFile(TokenPos()));
        }

        bool PerformTest()
        {
            REQUIRE(m_lexer);

            m_consumed_token_count = 0;
            for (const auto* sequence : m_scope_sequences)
            {
                const auto couldMatch = sequence->MatchSequence(m_lexer.get(), m_state.get(), m_consumed_token_count);
                if (couldMatch)
                    return couldMatch;
            }

            return false;
        }
    };

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use fadeIn", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("fadeIn")),
            SimpleParserValue::String(pos, new std::string("some_element")),
            SimpleParserValue::Character(pos, ';'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto script = helper.m_state->m_current_script.str();
        REQUIRE(script == R"("fadeIn" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Keyword casing doesnt matter", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("fadein")),
            SimpleParserValue::String(pos, new std::string("some_element")),
            SimpleParserValue::Character(pos, ';'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto script = helper.m_state->m_current_script.str();
        REQUIRE(script == R"("fadein" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Invalid keywords are not recognized", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("noScriptCommand")),
            SimpleParserValue::String(pos, new std::string("some_element")),
            SimpleParserValue::Character(pos, ';'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(!result);
        REQUIRE(helper.m_consumed_token_count == 0);
    }

    TEST_CASE("EventHandlerSetScopeSequences: Closing block terminates EventHandlerSet", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 1);

        const auto script = helper.m_state->m_current_script.str();
        REQUIRE(helper.m_state->m_current_event_handler_set == nullptr);
        REQUIRE(helper.m_state->m_current_nested_event_handler_set == nullptr);
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_event_handler_set->m_elements.empty());
    }

    TEST_CASE("EventHandlerSetScopeSequences: Closing block finishes current script", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::EndOfFile(pos)
        });

        helper.m_state->m_current_script << R"("fadeIn" "some_element" ; )";

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 1);
        REQUIRE(helper.m_state->m_current_event_handler_set == nullptr);
        REQUIRE(helper.m_state->m_current_nested_event_handler_set == nullptr);
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_event_handler_set->m_elements.size() == 1);

        const auto* firstElement = helper.m_event_handler_set->m_elements[0].get();

        REQUIRE(firstElement->GetType() == CommonEventHandlerElementType::SCRIPT);

        const auto* scriptElement = dynamic_cast<const CommonEventHandlerScript*>(firstElement);
        REQUIRE(scriptElement != nullptr);
        REQUIRE(scriptElement->m_script == R"("fadeIn" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: If opens new condition", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("if")),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 1336),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 1),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '{'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 7);
        REQUIRE(helper.m_state->m_current_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_event_handler_set->m_elements.size() == 1);

        const auto* firstElement = helper.m_event_handler_set->m_elements[0].get();

        REQUIRE(firstElement->GetType() == CommonEventHandlerElementType::CONDITION);

        const auto* conditionElement = dynamic_cast<const CommonEventHandlerCondition*>(firstElement);
        REQUIRE(conditionElement != nullptr);

        REQUIRE(conditionElement->m_condition != nullptr);
        REQUIRE(conditionElement->m_condition->IsStatic());
        const auto staticValue = conditionElement->m_condition->Evaluate();
        REQUIRE(staticValue.m_type == CommonExpressionValue::Type::INT);
        REQUIRE(staticValue.m_int_value == 1337);

        REQUIRE(conditionElement->m_condition_elements);
        REQUIRE(helper.m_state->m_current_nested_event_handler_set == conditionElement->m_condition_elements.get());
        REQUIRE(!conditionElement->m_else_elements);

        REQUIRE(helper.m_state->m_condition_stack.size() == 1);
        REQUIRE(helper.m_state->m_condition_stack.top().m_condition == conditionElement);
        REQUIRE(helper.m_state->m_condition_stack.top().m_auto_skip == false);
        REQUIRE(helper.m_state->m_condition_stack.top().m_in_condition_elements == true);
    }

    TEST_CASE("EventHandlerSetScopeSequences: If applies current script", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("if")),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '{'),
            SimpleParserValue::EndOfFile(pos)
        });

        helper.m_state->m_current_script << R"("fadeIn" "some_element" ; )";

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 5);
        REQUIRE(helper.m_state->m_current_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_event_handler_set->m_elements.size() == 2);

        const auto* firstElement = helper.m_event_handler_set->m_elements[0].get();

        REQUIRE(firstElement->GetType() == CommonEventHandlerElementType::SCRIPT);

        const auto* scriptElement = dynamic_cast<const CommonEventHandlerScript*>(firstElement);
        REQUIRE(scriptElement != nullptr);
        REQUIRE(scriptElement->m_script == R"("fadeIn" "some_element" ; )");

        const auto* secondElement = helper.m_event_handler_set->m_elements[1].get();

        REQUIRE(secondElement->GetType() == CommonEventHandlerElementType::CONDITION);

        const auto* conditionElement = dynamic_cast<const CommonEventHandlerCondition*>(secondElement);
        REQUIRE(conditionElement != nullptr);
    }

    TEST_CASE("EventHandlerSetScopeSequences: ElseIf opens new condition", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::Identifier(pos, new std::string("elseif")),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 1336),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 1),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '{'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto currentIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = currentIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(currentIfCondition.get());

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 8);
        REQUIRE(helper.m_state->m_current_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_event_handler_set->m_elements.empty()); // Doesnt add new element

        REQUIRE(currentIfCondition->m_else_elements);
        REQUIRE(currentIfCondition->m_else_elements->m_elements.size() == 1);

        const auto* firstElement = currentIfCondition->m_else_elements->m_elements[0].get();

        REQUIRE(firstElement->GetType() == CommonEventHandlerElementType::CONDITION);

        const auto* conditionElement = dynamic_cast<const CommonEventHandlerCondition*>(firstElement);
        REQUIRE(conditionElement != nullptr);

        REQUIRE(conditionElement->m_condition != nullptr);
        REQUIRE(conditionElement->m_condition->IsStatic());
        const auto staticValue = conditionElement->m_condition->Evaluate();
        REQUIRE(staticValue.m_type == CommonExpressionValue::Type::INT);
        REQUIRE(staticValue.m_int_value == 1337);

        REQUIRE(conditionElement->m_condition_elements);
        REQUIRE(helper.m_state->m_current_nested_event_handler_set == conditionElement->m_condition_elements.get());
        REQUIRE(!conditionElement->m_else_elements);

        REQUIRE(helper.m_state->m_condition_stack.size() == 2);
        REQUIRE(helper.m_state->m_condition_stack.top().m_condition == conditionElement);
        REQUIRE(helper.m_state->m_condition_stack.top().m_auto_skip == true);
        REQUIRE(helper.m_state->m_condition_stack.top().m_in_condition_elements == true);
    }

    TEST_CASE("EventHandlerSetScopeSequences: ElseIf applies current script", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::Identifier(pos, new std::string("elseif")),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '{'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto currentIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = currentIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(currentIfCondition.get());

        helper.m_state->m_current_script << R"("fadeIn" "some_element" ; )";

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 6);

        REQUIRE(currentIfCondition->m_condition_elements);
        REQUIRE(currentIfCondition->m_condition_elements->m_elements.size() == 1);

        const auto* firstConditionElement = currentIfCondition->m_condition_elements->m_elements[0].get();

        REQUIRE(firstConditionElement->GetType() == CommonEventHandlerElementType::SCRIPT);

        const auto* scriptElement = dynamic_cast<const CommonEventHandlerScript*>(firstConditionElement);
        REQUIRE(scriptElement != nullptr);
        REQUIRE(scriptElement->m_script == R"("fadeIn" "some_element" ; )");

        REQUIRE(currentIfCondition->m_else_elements);
        REQUIRE(currentIfCondition->m_else_elements->m_elements.size() == 1);

        const auto* firstElseElement = currentIfCondition->m_else_elements->m_elements[0].get();

        REQUIRE(firstElseElement->GetType() == CommonEventHandlerElementType::CONDITION);
    }

    TEST_CASE("EventHandlerSetScopeSequences: ElseIf cannot be specified when not in if", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::Identifier(pos, new std::string("elseif")),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '{'),
            SimpleParserValue::EndOfFile(pos)
        });

        REQUIRE_THROWS_AS(helper.PerformTest(), ParsingException);
    }

    TEST_CASE("EventHandlerSetScopeSequences: ElseIf cannot be specified when else was already used", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::Identifier(pos, new std::string("elseif")),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '{'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto currentIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), std::make_unique<CommonEventHandlerSet>());
        helper.m_state->m_current_nested_event_handler_set = currentIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(currentIfCondition.get());
        helper.m_state->m_condition_stack.top().m_in_condition_elements = false;

        REQUIRE_THROWS_AS(helper.PerformTest(), ParsingException);
    }

    TEST_CASE("EventHandlerSetScopeSequences: Else switches to else element", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::Identifier(pos, new std::string("else")),
            SimpleParserValue::Character(pos, '{'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto currentIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = currentIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(currentIfCondition.get());

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);
        REQUIRE(helper.m_state->m_current_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_event_handler_set->m_elements.empty()); // Doesn't add new element

        REQUIRE(currentIfCondition->m_else_elements);
        REQUIRE(currentIfCondition->m_else_elements->m_elements.empty());

        REQUIRE(helper.m_state->m_condition_stack.size() == 1);
        REQUIRE(helper.m_state->m_condition_stack.top().m_condition == currentIfCondition.get());
        REQUIRE(helper.m_state->m_condition_stack.top().m_auto_skip == false);
        REQUIRE(helper.m_state->m_condition_stack.top().m_in_condition_elements == false);
    }

    TEST_CASE("EventHandlerSetScopeSequences: Else keeps auto_skip value", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::Identifier(pos, new std::string("else")),
            SimpleParserValue::Character(pos, '{'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto currentIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = currentIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(currentIfCondition.get(), true);

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);
        REQUIRE(helper.m_state->m_current_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_event_handler_set->m_elements.empty()); // Doesn't add new element

        REQUIRE(currentIfCondition->m_else_elements);
        REQUIRE(currentIfCondition->m_else_elements->m_elements.empty());

        REQUIRE(helper.m_state->m_condition_stack.size() == 1);
        REQUIRE(helper.m_state->m_condition_stack.top().m_condition == currentIfCondition.get());
        REQUIRE(helper.m_state->m_condition_stack.top().m_auto_skip == true);
        REQUIRE(helper.m_state->m_condition_stack.top().m_in_condition_elements == false);
    }

    TEST_CASE("EventHandlerSetScopeSequences: Else cannot be specified when not in if", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::Identifier(pos, new std::string("else")),
            SimpleParserValue::Character(pos, '{'),
            SimpleParserValue::EndOfFile(pos)
        });

        REQUIRE_THROWS_AS(helper.PerformTest(), ParsingException);
    }

    TEST_CASE("EventHandlerSetScopeSequences: Else cannot be specified when else was already used", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::Identifier(pos, new std::string("else")),
            SimpleParserValue::Character(pos, '{'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto currentIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), std::make_unique<CommonEventHandlerSet>());
        helper.m_state->m_current_nested_event_handler_set = currentIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(currentIfCondition.get());
        helper.m_state->m_condition_stack.top().m_in_condition_elements = false;

        REQUIRE_THROWS_AS(helper.PerformTest(), ParsingException);
    }

    TEST_CASE("EventHandlerSetScopeSequences: CloseBlock closes if statements", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto currentIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = currentIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(currentIfCondition.get());

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 1);
        REQUIRE(helper.m_state->m_current_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_event_handler_set->m_elements.empty()); // Doesn't add new element

        REQUIRE(helper.m_state->m_current_nested_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_state->m_condition_stack.empty());
    }

    TEST_CASE("EventHandlerSetScopeSequences: CloseBlock closes nested if statements to parent if statement", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto baseIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = baseIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(baseIfCondition.get());

        const auto nestedIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = nestedIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(nestedIfCondition.get());

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 1);
        REQUIRE(helper.m_state->m_current_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_state->m_current_nested_event_handler_set->m_elements.empty()); // Doesn't add new element

        REQUIRE(helper.m_state->m_current_nested_event_handler_set == baseIfCondition->m_condition_elements.get());
        REQUIRE(helper.m_state->m_condition_stack.size() == 1);
    }

    TEST_CASE("EventHandlerSetScopeSequences: CloseBlock closes nested if statements to parent else statement", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto baseIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), std::make_unique<CommonEventHandlerSet>());
        helper.m_state->m_current_nested_event_handler_set = baseIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(baseIfCondition.get());
        helper.m_state->m_condition_stack.top().m_in_condition_elements = false;

        const auto nestedIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = nestedIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(nestedIfCondition.get());

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 1);
        REQUIRE(helper.m_state->m_current_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_state->m_current_nested_event_handler_set->m_elements.empty()); // Doesn't add new element

        REQUIRE(helper.m_state->m_current_nested_event_handler_set == baseIfCondition->m_else_elements.get());
        REQUIRE(helper.m_state->m_condition_stack.size() == 1);
    }

    TEST_CASE("EventHandlerSetScopeSequences: CloseBlock closes all autoskip conditions to parent if statement", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto baseIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = baseIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(baseIfCondition.get());

        const auto nestedIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = nestedIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(nestedIfCondition.get());

        const auto moreNestedIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = moreNestedIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(moreNestedIfCondition.get(), true);

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 1);
        REQUIRE(helper.m_state->m_current_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_state->m_current_nested_event_handler_set->m_elements.empty()); // Doesn't add new element

        REQUIRE(helper.m_state->m_current_nested_event_handler_set == baseIfCondition->m_condition_elements.get());
        REQUIRE(helper.m_state->m_condition_stack.size() == 1);
    }

    TEST_CASE("EventHandlerSetScopeSequences: CloseBlock closes all autoskip conditions to base", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto baseIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = baseIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(baseIfCondition.get());

        const auto nestedIfCondition = std::make_unique<CommonEventHandlerCondition>(nullptr, std::make_unique<CommonEventHandlerSet>(), nullptr);
        helper.m_state->m_current_nested_event_handler_set = nestedIfCondition->m_condition_elements.get();
        helper.m_state->m_condition_stack.emplace(nestedIfCondition.get(), true);

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 1);
        REQUIRE(helper.m_state->m_current_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_event_handler_set);
        REQUIRE(helper.m_state->m_current_nested_event_handler_set->m_elements.empty()); // Doesn't add new element

        REQUIRE(helper.m_state->m_current_nested_event_handler_set == helper.m_event_handler_set.get());
        REQUIRE(helper.m_state->m_condition_stack.empty());
    }
}
