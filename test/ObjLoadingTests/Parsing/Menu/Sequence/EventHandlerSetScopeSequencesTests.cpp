#include <catch2/catch.hpp>

#include "Parsing/Menu/Domain/EventHandler/CommonEventHandlerScript.h"
#include "Parsing/Menu/Domain/EventHandler/CommonEventHandlerSetLocalVar.h"
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

        explicit EventHandlerSetSequenceTestsHelper(FeatureLevel featureLevel, bool permissive)
            : m_state(std::make_unique<MenuFileParserState>(featureLevel, false)),
              m_event_handler_set(std::make_unique<CommonEventHandlerSet>()),
              m_consumed_token_count(0u)
        {
            EventHandlerSetScopeSequences scopeSequences(m_all_sequences, m_scope_sequences);
            scopeSequences.AddSequences(m_state->m_feature_level, permissive);

            m_state->m_current_menu = m_state->m_menus.emplace_back(std::make_unique<CommonMenuDef>()).get();
            m_state->m_current_event_handler_set = m_event_handler_set.get();
            m_state->m_current_nested_event_handler_set = m_event_handler_set.get();
        }

        void Tokens(std::initializer_list<Movable<SimpleParserValue>> tokens)
        {
            m_lexer = std::make_unique<MockLexer<SimpleParserValue>>(tokens, SimpleParserValue::EndOfFile(TokenPos()));
        }

        void Tokens(std::vector<SimpleParserValue> tokens)
        {
            m_lexer = std::make_unique<MockLexer<SimpleParserValue>>(std::move(tokens), SimpleParserValue::EndOfFile(TokenPos()));
        }

        bool PerformTest()
        {
            REQUIRE(m_lexer);

            m_consumed_token_count = 0;
            for (const auto* sequence : m_scope_sequences)
            {
                const auto couldMatch = sequence->MatchSequence(m_lexer.get(), m_state.get(), m_consumed_token_count);
                if (couldMatch)
                {
                    m_lexer->PopTokens(static_cast<int>(m_consumed_token_count));
                    return couldMatch;
                }
            }

            return false;
        }
    };

#pragma region General
    TEST_CASE("EventHandlerSetScopeSequences: Keyword casing doesnt matter", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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

    TEST_CASE("EventHandlerSetScopeSequences: Permissive mode ignores unknown script tokens and adds them to script", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, true);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("uiScript")),
            SimpleParserValue::Identifier(pos, new std::string("somethingUnknown")),
            SimpleParserValue::String(pos, new std::string("anArgumentForTheUnknownScript")),
            SimpleParserValue::Character(pos, ';'),
            SimpleParserValue::EndOfFile(pos)
        });

        auto result = helper.PerformTest();
        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 1);
        auto script = helper.m_state->m_current_script.str();
        REQUIRE(script == R"("uiScript" )");

        result = helper.PerformTest();
        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 1);
        script = helper.m_state->m_current_script.str();
        REQUIRE(script == R"("uiScript" "somethingUnknown" )");

        result = helper.PerformTest();
        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 1);
        script = helper.m_state->m_current_script.str();
        REQUIRE(script == R"("uiScript" "somethingUnknown" "anArgumentForTheUnknownScript" )");

        result = helper.PerformTest();
        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 1);
        script = helper.m_state->m_current_script.str();
        REQUIRE(script == R"("uiScript" "somethingUnknown" "anArgumentForTheUnknownScript" ; )");
    }

#pragma endregion

    void TestGenericScriptStatement(const std::initializer_list<Movable<SimpleParserValue>> tokens, const std::string& expectedScript)
    {
        std::vector<SimpleParserValue> tokenList(std::make_move_iterator(tokens.begin()), std::make_move_iterator(tokens.end()));
        const auto initialValueCount = tokenList.size();

        tokenList.emplace_back(SimpleParserValue::Character(TokenPos(), ';'));
        tokenList.emplace_back(SimpleParserValue::EndOfFile(TokenPos()));

        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
        helper.Tokens(std::move(tokenList));

        const auto result = helper.PerformTest();
        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == initialValueCount + 1);

        const auto script = helper.m_state->m_current_script.str();
        REQUIRE(script == expectedScript);
    }

#pragma region Command Tests

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use fadeIn", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("fadeIn")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("fadeIn" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use fadeOut", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("fadeOut")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("fadeOut" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use show", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("show")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("show" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use hide", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("hide")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("hide" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use showMenu", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("showMenu")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("showMenu" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use hideMenu", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("hideMenu")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("hideMenu" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setColor", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setColor")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
            }, R"("setColor" "1" "1" "1" "1" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setColor with 3 colors", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setColor")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
            }, R"("setColor" "1" "1" "1" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setColor with 2 colors", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setColor")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
            }, R"("setColor" "1" "1" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setColor with 1 color", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setColor")),
                SimpleParserValue::Integer(TokenPos(), 1),
            }, R"("setColor" "1" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure cannot use setColor with no color", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("setColor")),
            SimpleParserValue::Character(pos, ';'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result == false);
        REQUIRE(helper.m_consumed_token_count == 0);
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use open", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("open")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("open" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use close", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("close")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("close" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use escape", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("escape")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("escape" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use closeForAllPlayers", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("closeForAllPlayers")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("closeForAllPlayers" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use ingameOpen", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("ingameOpen")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("ingameOpen" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use ingameClose", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("ingameClose")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("ingameClose" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setBackground", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setBackground")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("setBackground" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setItemColor with forecolor", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setItemColor")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element")),
                SimpleParserValue::String(TokenPos(), new std::string("forecolor")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
            }, R"("setItemColor" "some_element" "forecolor" "1" "1" "1" "1" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setItemColor with backcolor", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setItemColor")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element")),
                SimpleParserValue::String(TokenPos(), new std::string("backcolor")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
            }, R"("setItemColor" "some_element" "backcolor" "1" "1" "1" "1" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setItemColor with bordercolor", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setItemColor")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element")),
                SimpleParserValue::String(TokenPos(), new std::string("bordercolor")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Integer(TokenPos(), 1),
            }, R"("setItemColor" "some_element" "bordercolor" "1" "1" "1" "1" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use focusFirst", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("focusFirst")),
            }, R"("focusFirst" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setFocus", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setFocus")),
                SimpleParserValue::String(TokenPos(), new std::string("some_element"))
            }, R"("setFocus" "some_element" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setFocusByDvar", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setFocusByDvar")),
                SimpleParserValue::String(TokenPos(), new std::string("some_dvar"))
            }, R"("setFocusByDvar" "some_dvar" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setDvar", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setDvar")),
                SimpleParserValue::String(TokenPos(), new std::string("some_dvar")),
                SimpleParserValue::String(TokenPos(), new std::string("some_value")),
            }, R"("setDvar" "some_dvar" "some_value" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use exec", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("exec")),
                SimpleParserValue::String(TokenPos(), new std::string("some_command"))
            }, R"("exec" "some_command" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use execNow", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("execNow")),
                SimpleParserValue::String(TokenPos(), new std::string("some_command"))
            }, R"("execNow" "some_command" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use execOnDvarStringValue", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("execOnDvarStringValue")),
                SimpleParserValue::String(TokenPos(), new std::string("some_dvar")),
                SimpleParserValue::String(TokenPos(), new std::string("some_value")),
                SimpleParserValue::String(TokenPos(), new std::string("some_command"))
            }, R"("execOnDvarStringValue" "some_dvar" "some_value" "some_command" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use execOnDvarIntValue", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("execOnDvarIntValue")),
                SimpleParserValue::String(TokenPos(), new std::string("some_dvar")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::String(TokenPos(), new std::string("some_command"))
            }, R"("execOnDvarIntValue" "some_dvar" "1" "some_command" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use execOnDvarFloatValue", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("execOnDvarFloatValue")),
                SimpleParserValue::String(TokenPos(), new std::string("some_dvar")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::String(TokenPos(), new std::string("some_command"))
            }, R"("execOnDvarFloatValue" "some_dvar" "1" "some_command" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use execNowOnDvarStringValue", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("execNowOnDvarStringValue")),
                SimpleParserValue::String(TokenPos(), new std::string("some_dvar")),
                SimpleParserValue::String(TokenPos(), new std::string("some_value")),
                SimpleParserValue::String(TokenPos(), new std::string("some_command"))
            }, R"("execNowOnDvarStringValue" "some_dvar" "some_value" "some_command" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use execNowOnDvarIntValue", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("execNowOnDvarIntValue")),
                SimpleParserValue::String(TokenPos(), new std::string("some_dvar")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::String(TokenPos(), new std::string("some_command"))
            }, R"("execNowOnDvarIntValue" "some_dvar" "1" "some_command" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use execNowOnDvarFloatValue", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("execNowOnDvarFloatValue")),
                SimpleParserValue::String(TokenPos(), new std::string("some_dvar")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::String(TokenPos(), new std::string("some_command"))
            }, R"("execNowOnDvarFloatValue" "some_dvar" "1" "some_command" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use play", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("play")),
                SimpleParserValue::String(TokenPos(), new std::string("some_sound"))
            }, R"("play" "some_sound" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use scriptMenuResponse", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("scriptMenuResponse")),
                SimpleParserValue::String(TokenPos(), new std::string("some_response"))
            }, R"("scriptMenuResponse" "some_response" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use respondOnDvarStringValue", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("respondOnDvarStringValue")),
                SimpleParserValue::String(TokenPos(), new std::string("some_dvar")),
                SimpleParserValue::String(TokenPos(), new std::string("some_value")),
                SimpleParserValue::String(TokenPos(), new std::string("some_response"))
            }, R"("respondOnDvarStringValue" "some_dvar" "some_value" "some_response" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use respondOnDvarIntValue", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("respondOnDvarIntValue")),
                SimpleParserValue::String(TokenPos(), new std::string("some_dvar")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::String(TokenPos(), new std::string("some_response"))
            }, R"("respondOnDvarIntValue" "some_dvar" "1" "some_response" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use respondOnDvarFloatValue", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("respondOnDvarFloatValue")),
                SimpleParserValue::String(TokenPos(), new std::string("some_dvar")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::String(TokenPos(), new std::string("some_response"))
            }, R"("respondOnDvarFloatValue" "some_dvar" "1" "some_response" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setPlayerDataSp", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setPlayerDataSp"))
            }, R"("setPlayerDataSp" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use updateMail", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("updateMail"))
            }, R"("updateMail" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use openMail", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("openMail"))
            }, R"("openMail" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use deleteMail", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("deleteMail"))
            }, R"("deleteMail" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use doMailLottery", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("doMailLottery"))
            }, R"("doMailLottery" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use resetStatsConfirm", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("resetStatsConfirm"))
            }, R"("resetStatsConfirm" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use resetStatsCancel", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("resetStatsCancel"))
            }, R"("resetStatsCancel" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setGameMode", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setGameMode")),
                SimpleParserValue::String(TokenPos(), new std::string("some_game_mode"))
            }, R"("setGameMode" "some_game_mode" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use feederTop", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("feederTop"))
            }, R"("feederTop" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use feederBottom", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("feederBottom"))
            }, R"("feederBottom" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use showGamerCard", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("showGamerCard"))
            }, R"("showGamerCard" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use openForGameType", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("openForGameType")),
                SimpleParserValue::String(TokenPos(), new std::string("some_game_type"))
            }, R"("openForGameType" "some_game_type" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use closeForGameType", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("closeForGameType")),
                SimpleParserValue::String(TokenPos(), new std::string("some_game_type"))
            }, R"("closeForGameType" "some_game_type" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use kickPlayer", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("kickPlayer"))
            }, R"("kickPlayer" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use getKickPlayerQuestion", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("getKickPlayerQuestion"))
            }, R"("getKickPlayerQuestion" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use partyUpdateMissingMapPackDvar", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("partyUpdateMissingMapPackDvar"))
            }, R"("partyUpdateMissingMapPackDvar" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use togglePlayerMute", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("togglePlayerMute"))
            }, R"("togglePlayerMute" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use resolveError", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("resolveError"))
            }, R"("resolveError" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use lerp scale", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("lerp")),
                SimpleParserValue::Identifier(TokenPos(), new std::string("scale")),
                SimpleParserValue::Identifier(TokenPos(), new std::string("from")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Identifier(TokenPos(), new std::string("to")),
                SimpleParserValue::Integer(TokenPos(), 2),
                SimpleParserValue::Identifier(TokenPos(), new std::string("over")),
                SimpleParserValue::Integer(TokenPos(), 3),
            }, R"("lerp" "scale" "from" "1" "to" "2" "over" "3" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use lerp alpha", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("lerp")),
                SimpleParserValue::Identifier(TokenPos(), new std::string("alpha")),
                SimpleParserValue::Identifier(TokenPos(), new std::string("from")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Identifier(TokenPos(), new std::string("to")),
                SimpleParserValue::Integer(TokenPos(), 2),
                SimpleParserValue::Identifier(TokenPos(), new std::string("over")),
                SimpleParserValue::Integer(TokenPos(), 3),
            }, R"("lerp" "alpha" "from" "1" "to" "2" "over" "3" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use lerp x", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("lerp")),
                SimpleParserValue::Identifier(TokenPos(), new std::string("x")),
                SimpleParserValue::Identifier(TokenPos(), new std::string("from")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Identifier(TokenPos(), new std::string("to")),
                SimpleParserValue::Integer(TokenPos(), 2),
                SimpleParserValue::Identifier(TokenPos(), new std::string("over")),
                SimpleParserValue::Integer(TokenPos(), 3),
            }, R"("lerp" "x" "from" "1" "to" "2" "over" "3" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use lerp y", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("lerp")),
                SimpleParserValue::Identifier(TokenPos(), new std::string("y")),
                SimpleParserValue::Identifier(TokenPos(), new std::string("from")),
                SimpleParserValue::Integer(TokenPos(), 1),
                SimpleParserValue::Identifier(TokenPos(), new std::string("to")),
                SimpleParserValue::Integer(TokenPos(), 2),
                SimpleParserValue::Identifier(TokenPos(), new std::string("over")),
                SimpleParserValue::Integer(TokenPos(), 3),
            }, R"("lerp" "y" "from" "1" "to" "2" "over" "3" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure can use setPlayerData", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setPlayerData")),
                SimpleParserValue::Character(TokenPos(), '('),
                SimpleParserValue::Identifier(TokenPos(), new std::string("customClasses")),
                SimpleParserValue::Character(TokenPos(), ','),
                SimpleParserValue::Identifier(TokenPos(), new std::string("localVarInt")),
                SimpleParserValue::Character(TokenPos(), '('),
                SimpleParserValue::Identifier(TokenPos(), new std::string("classIndex")),
                SimpleParserValue::Character(TokenPos(), ')'),
                SimpleParserValue::Character(TokenPos(), ','),
                SimpleParserValue::String(TokenPos(), new std::string("perks")),
                SimpleParserValue::Character(TokenPos(), ','),
                SimpleParserValue::Integer(TokenPos(), 0),
                SimpleParserValue::Character(TokenPos(), ','),
                SimpleParserValue::Identifier(TokenPos(), new std::string("speciality_null")),
                SimpleParserValue::Character(TokenPos(), ')'),
            }, R"("setPlayerData" ( "customClasses" , "localVarInt" ( "classIndex" ) , "perks" , "0" , "speciality_null" ) ; )");
    }

#pragma endregion

#pragma region Unit Tests for setLocalVar

    TEST_CASE("EventHandlerSetScopeSequences: Ensure setLocalVarBool is script on static value", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setLocalVarBool")),
                SimpleParserValue::Identifier(TokenPos(), new std::string("sample_var")),
                SimpleParserValue::Integer(TokenPos(), 1339),
                SimpleParserValue::Character(TokenPos(), '-'),
                SimpleParserValue::Integer(TokenPos(), 2),
            }, R"("setLocalVarBool" "sample_var" "1337" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure setLocalVarInt is script on static value", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setLocalVarInt")),
                SimpleParserValue::Identifier(TokenPos(), new std::string("sample_var")),
                SimpleParserValue::Integer(TokenPos(), 1339),
                SimpleParserValue::Character(TokenPos(), '-'),
                SimpleParserValue::Integer(TokenPos(), 2),
            }, R"("setLocalVarInt" "sample_var" "1337" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure setLocalVarString is script on static value", "[parsing][sequence][menu]")
    {
        TestGenericScriptStatement(
            {
                SimpleParserValue::Identifier(TokenPos(), new std::string("setLocalVarString")),
                SimpleParserValue::Identifier(TokenPos(), new std::string("sample_var")),
                SimpleParserValue::String(TokenPos(), new std::string("Hello")),
                SimpleParserValue::Character(TokenPos(), '+'),
                SimpleParserValue::String(TokenPos(), new std::string(" ")),
                SimpleParserValue::Character(TokenPos(), '+'),
                SimpleParserValue::String(TokenPos(), new std::string("World")),
            }, R"("setLocalVarString" "sample_var" "Hello World" ; )");
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure setLocalVarBool is setLocalVar handler on non-static value", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(TokenPos(), new std::string("setLocalVarBool")),
            SimpleParserValue::Identifier(TokenPos(), new std::string("sample_var")),
            SimpleParserValue::Identifier(TokenPos(), new std::string("milliseconds")),
            SimpleParserValue::Character(TokenPos(), '('),
            SimpleParserValue::Character(TokenPos(), ')'),
            SimpleParserValue::Character(TokenPos(), '%'),
            SimpleParserValue::Integer(TokenPos(), 2),
            SimpleParserValue::Character(TokenPos(), ';'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();
        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 8);

        REQUIRE(helper.m_event_handler_set->m_elements.size() == 1);

        const auto* firstElement = helper.m_event_handler_set->m_elements[0].get();

        REQUIRE(firstElement->GetType() == CommonEventHandlerElementType::SET_LOCAL_VAR);

        const auto* setLocalVarElement = dynamic_cast<const CommonEventHandlerSetLocalVar*>(firstElement);
        REQUIRE(setLocalVarElement != nullptr);

        REQUIRE(setLocalVarElement->m_type == SetLocalVarType::BOOL);
        REQUIRE(setLocalVarElement->m_var_name == "sample_var");
        REQUIRE(setLocalVarElement->m_value);
        REQUIRE(setLocalVarElement->m_value->IsStatic() == false);
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure setLocalVarInt is setLocalVar handler on non-static value", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(TokenPos(), new std::string("setLocalVarInt")),
            SimpleParserValue::Identifier(TokenPos(), new std::string("sample_var")),
            SimpleParserValue::Identifier(TokenPos(), new std::string("milliseconds")),
            SimpleParserValue::Character(TokenPos(), '('),
            SimpleParserValue::Character(TokenPos(), ')'),
            SimpleParserValue::Character(TokenPos(), '*'),
            SimpleParserValue::Integer(TokenPos(), 2),
            SimpleParserValue::Character(TokenPos(), ';'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();
        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 8);

        REQUIRE(helper.m_event_handler_set->m_elements.size() == 1);

        const auto* firstElement = helper.m_event_handler_set->m_elements[0].get();

        REQUIRE(firstElement->GetType() == CommonEventHandlerElementType::SET_LOCAL_VAR);

        const auto* setLocalVarElement = dynamic_cast<const CommonEventHandlerSetLocalVar*>(firstElement);
        REQUIRE(setLocalVarElement != nullptr);

        REQUIRE(setLocalVarElement->m_type == SetLocalVarType::INT);
        REQUIRE(setLocalVarElement->m_var_name == "sample_var");
        REQUIRE(setLocalVarElement->m_value);
        REQUIRE(setLocalVarElement->m_value->IsStatic() == false);
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure setLocalVarFloat is setLocalVar handler on non-static value", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(TokenPos(), new std::string("setLocalVarFloat")),
            SimpleParserValue::Identifier(TokenPos(), new std::string("sample_var")),
            SimpleParserValue::Identifier(TokenPos(), new std::string("milliseconds")),
            SimpleParserValue::Character(TokenPos(), '('),
            SimpleParserValue::Character(TokenPos(), ')'),
            SimpleParserValue::Character(TokenPos(), '/'),
            SimpleParserValue::Integer(TokenPos(), 2),
            SimpleParserValue::Character(TokenPos(), ';'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();
        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 8);

        REQUIRE(helper.m_event_handler_set->m_elements.size() == 1);

        const auto* firstElement = helper.m_event_handler_set->m_elements[0].get();

        REQUIRE(firstElement->GetType() == CommonEventHandlerElementType::SET_LOCAL_VAR);

        const auto* setLocalVarElement = dynamic_cast<const CommonEventHandlerSetLocalVar*>(firstElement);
        REQUIRE(setLocalVarElement != nullptr);

        REQUIRE(setLocalVarElement->m_type == SetLocalVarType::FLOAT);
        REQUIRE(setLocalVarElement->m_var_name == "sample_var");
        REQUIRE(setLocalVarElement->m_value);
        REQUIRE(setLocalVarElement->m_value->IsStatic() == false);
    }

    TEST_CASE("EventHandlerSetScopeSequences: Ensure setLocalVarString is setLocalVar handler on non-static value", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(TokenPos(), new std::string("setLocalVarString")),
            SimpleParserValue::Identifier(TokenPos(), new std::string("sample_var")),
            SimpleParserValue::Identifier(TokenPos(), new std::string("milliseconds")),
            SimpleParserValue::Character(TokenPos(), '('),
            SimpleParserValue::Character(TokenPos(), ')'),
            SimpleParserValue::Character(TokenPos(), '+'),
            SimpleParserValue::String(TokenPos(), new std::string(" Hello")),
            SimpleParserValue::Character(TokenPos(), ';'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();
        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 8);

        REQUIRE(helper.m_event_handler_set->m_elements.size() == 1);

        const auto* firstElement = helper.m_event_handler_set->m_elements[0].get();

        REQUIRE(firstElement->GetType() == CommonEventHandlerElementType::SET_LOCAL_VAR);

        const auto* setLocalVarElement = dynamic_cast<const CommonEventHandlerSetLocalVar*>(firstElement);
        REQUIRE(setLocalVarElement != nullptr);

        REQUIRE(setLocalVarElement->m_type == SetLocalVarType::STRING);
        REQUIRE(setLocalVarElement->m_var_name == "sample_var");
        REQUIRE(setLocalVarElement->m_value);
        REQUIRE(setLocalVarElement->m_value->IsStatic() == false);
    }

#pragma endregion

#pragma region Unit Tests for If/ElseIf/Else/CloseBracket

    TEST_CASE("EventHandlerSetScopeSequences: Closing block terminates EventHandlerSet", "[parsing][sequence][menu]")
    {
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        REQUIRE(staticValue.m_type == SimpleExpressionValue::Type::INT);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        REQUIRE(staticValue.m_type == SimpleExpressionValue::Type::INT);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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
        EventHandlerSetSequenceTestsHelper helper(FeatureLevel::IW4, false);
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

#pragma endregion
}
