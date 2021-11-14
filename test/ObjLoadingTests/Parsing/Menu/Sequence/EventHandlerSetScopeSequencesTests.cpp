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
}
