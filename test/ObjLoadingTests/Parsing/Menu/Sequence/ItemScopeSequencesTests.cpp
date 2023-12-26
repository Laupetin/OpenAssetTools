#include "Parsing/Menu/Sequence/ItemScopeSequences.h"
#include "Parsing/Mock/MockLexer.h"
#include "Utils/ClassUtils.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace menu;
using namespace Catch::Matchers;

namespace test::parsing::menu::sequence::item
{
    class ItemSequenceTestsHelper
    {
    public:
        std::vector<std::unique_ptr<MenuFileParser::sequence_t>> m_all_sequences;
        std::vector<MenuFileParser::sequence_t*> m_scope_sequences;
        std::unique_ptr<MenuFileParserState> m_state;
        std::unique_ptr<ILexer<SimpleParserValue>> m_lexer;

        std::unique_ptr<CommonItemDef> m_item;

        unsigned m_consumed_token_count;

        explicit ItemSequenceTestsHelper(FeatureLevel featureLevel, const bool permissive)
            : m_state(std::make_unique<MenuFileParserState>(featureLevel, false)),
              m_item(std::make_unique<CommonItemDef>()),
              m_consumed_token_count(0u)
        {
            ItemScopeSequences scopeSequences(m_all_sequences, m_scope_sequences);
            scopeSequences.AddSequences(m_state->m_feature_level, permissive);

            m_state->m_current_menu = m_state->m_menus.emplace_back(std::make_unique<CommonMenuDef>()).get();
            m_state->m_current_item = m_item.get();
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

    TEST_CASE("ItemScopeSequences: Can use static expressions for simple text properties", "[parsing][sequence][menu]")
    {
        ItemSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("name")),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::String(pos, new std::string("Hello")),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::String(pos, new std::string(" ")),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::String(pos, new std::string("World")),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::EndOfFile(pos),
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 8);

        const auto* item = helper.m_state->m_current_item;
        REQUIRE(item);

        REQUIRE(item->m_name == "Hello World");
    }

    TEST_CASE("ItemScopeSequences: Can use static expressions for simple int properties", "[parsing][sequence][menu]")
    {
        ItemSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("style")),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 1),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 2),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 6),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::EndOfFile(pos),
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 8);

        const auto* item = helper.m_state->m_current_item;
        REQUIRE(item);

        REQUIRE(item->m_style == 9);
    }

    TEST_CASE("ItemScopeSequences: Can use static expressions for simple float properties", "[parsing][sequence][menu]")
    {
        ItemSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("borderSize")),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::FloatingPoint(pos, 4.2),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::FloatingPoint(pos, 5),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::FloatingPoint(pos, 0.2),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::EndOfFile(pos),
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 8);

        const auto* item = helper.m_state->m_current_item;
        REQUIRE(item);

        REQUIRE_THAT(item->m_border_size, WithinRel(9.4));
    }

    TEST_CASE("ItemScopeSequences: Rect works with only x,y,w,h as ints", "[parsing][sequence][menu]")
    {
        ItemSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("rect")),
            SimpleParserValue::Integer(pos, 20),
            SimpleParserValue::Integer(pos, 40),
            SimpleParserValue::Integer(pos, 100),
            SimpleParserValue::Integer(pos, 520),
            SimpleParserValue::EndOfFile(pos),
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 5);

        const auto* item = helper.m_state->m_current_item;
        REQUIRE(item);

        REQUIRE_THAT(item->m_rect.x, WithinRel(20.0));
        REQUIRE_THAT(item->m_rect.y, WithinRel(40.0));
        REQUIRE_THAT(item->m_rect.w, WithinRel(100.0));
        REQUIRE_THAT(item->m_rect.h, WithinRel(520.0));
        REQUIRE(item->m_rect.horizontalAlign == 0);
        REQUIRE(item->m_rect.verticalAlign == 0);
    }

    TEST_CASE("ItemScopeSequences: Rect works with only x,y,w,h as floats", "[parsing][sequence][menu]")
    {
        ItemSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("rect")),
            SimpleParserValue::FloatingPoint(pos, 20.2),
            SimpleParserValue::FloatingPoint(pos, 40.4),
            SimpleParserValue::FloatingPoint(pos, 100.6),
            SimpleParserValue::FloatingPoint(pos, 520.8),
            SimpleParserValue::EndOfFile(pos),
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 5);

        const auto* item = helper.m_state->m_current_item;
        REQUIRE(item);

        REQUIRE_THAT(item->m_rect.x, WithinRel(20.2));
        REQUIRE_THAT(item->m_rect.y, WithinRel(40.4));
        REQUIRE_THAT(item->m_rect.w, WithinRel(100.6));
        REQUIRE_THAT(item->m_rect.h, WithinRel(520.8));
    }

    TEST_CASE("ItemScopeSequences: Rect supports expressions", "[parsing][sequence][menu]")
    {
        ItemSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("rect")),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::FloatingPoint(pos, 20.2),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 40),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::FloatingPoint(pos, 40.2),
            SimpleParserValue::Character(pos, '-'),
            SimpleParserValue::FloatingPoint(pos, 1.4),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::FloatingPoint(pos, 100.6),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::FloatingPoint(pos, 2.0),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::FloatingPoint(pos, 5.0),
            SimpleParserValue::Character(pos, '*'),
            SimpleParserValue::FloatingPoint(pos, 1.5),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::EndOfFile(pos),
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 21);

        const auto* item = helper.m_state->m_current_item;
        REQUIRE(item);

        REQUIRE_THAT(item->m_rect.x, WithinRel(60.2));
        REQUIRE_THAT(item->m_rect.y, WithinRel(38.8));
        REQUIRE_THAT(item->m_rect.w, WithinRel(102.6));
        REQUIRE_THAT(item->m_rect.h, WithinRel(7.5));
    }

    TEST_CASE("ItemScopeSequences: Rect can specify align", "[parsing][sequence][menu]")
    {
        ItemSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("rect")),
            SimpleParserValue::FloatingPoint(pos, 20.2),
            SimpleParserValue::FloatingPoint(pos, 40.4),
            SimpleParserValue::FloatingPoint(pos, 100.6),
            SimpleParserValue::FloatingPoint(pos, 520.8),
            SimpleParserValue::Integer(pos, 1),
            SimpleParserValue::Integer(pos, 2),
            SimpleParserValue::EndOfFile(pos),
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 7);

        const auto* item = helper.m_state->m_current_item;
        REQUIRE(item);

        REQUIRE(item->m_rect.horizontalAlign == 1);
        REQUIRE(item->m_rect.verticalAlign == 2);
    }

    TEST_CASE("ItemScopeSequences: Can specify origin", "[parsing][sequence][menu]")
    {
        ItemSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("origin")),
            SimpleParserValue::FloatingPoint(pos, 4.20),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::FloatingPoint(pos, 11.37),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::FloatingPoint(pos, 2.0),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::EndOfFile(pos),
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 7);

        const auto* item = helper.m_state->m_current_item;
        REQUIRE(item);

        REQUIRE_THAT(item->m_rect.x, WithinRel(4.20));
        REQUIRE_THAT(item->m_rect.y, WithinRel(13.37));
        REQUIRE_THAT(item->m_rect.w, WithinRel(0.0));
        REQUIRE_THAT(item->m_rect.h, WithinRel(0.0));
        REQUIRE(item->m_rect.horizontalAlign == 0);
        REQUIRE(item->m_rect.verticalAlign == 0);
    }

    TEST_CASE("ItemScopeSequences: Simple dvarStrList works", "[parsing][sequence][menu]")
    {
        ItemSequenceTestsHelper helper(FeatureLevel::IW4, false);
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Identifier(pos, new std::string("dvarStrList")),
            SimpleParserValue::Character(pos, '{'),
            SimpleParserValue::String(pos, new std::string("@MENU_AUTO")),
            SimpleParserValue::String(pos, new std::string("auto")),
            SimpleParserValue::String(pos, new std::string("@MENU_STANDARD_4_3")),
            SimpleParserValue::String(pos, new std::string("standard")),
            SimpleParserValue::String(pos, new std::string("@MENU_WIDE_16_10")),
            SimpleParserValue::String(pos, new std::string("wide 16:10")),
            SimpleParserValue::String(pos, new std::string("@MENU_WIDE_16_9")),
            SimpleParserValue::String(pos, new std::string("wide 16:9")),
            SimpleParserValue::Character(pos, '}'),
            SimpleParserValue::EndOfFile(pos),
        });

        helper.m_item->m_feature_type = CommonItemFeatureType::MULTI_VALUE;
        helper.m_item->m_multi_value_features = std::make_unique<CommonItemFeaturesMultiValue>();

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 11);

        const auto* item = helper.m_state->m_current_item;
        REQUIRE(item);
        const auto* multiValueFeatures = item->m_multi_value_features.get();
        REQUIRE(multiValueFeatures);

        REQUIRE(multiValueFeatures->m_step_names.size() == 4);
        REQUIRE(multiValueFeatures->m_string_values.size() == 4);

        REQUIRE(multiValueFeatures->m_step_names[0] == "@MENU_AUTO");
        REQUIRE(multiValueFeatures->m_step_names[1] == "@MENU_STANDARD_4_3");
        REQUIRE(multiValueFeatures->m_step_names[2] == "@MENU_WIDE_16_10");
        REQUIRE(multiValueFeatures->m_step_names[3] == "@MENU_WIDE_16_9");

        REQUIRE(multiValueFeatures->m_string_values[0] == "auto");
        REQUIRE(multiValueFeatures->m_string_values[1] == "standard");
        REQUIRE(multiValueFeatures->m_string_values[2] == "wide 16:10");
        REQUIRE(multiValueFeatures->m_string_values[3] == "wide 16:9");
    }
} // namespace test::parsing::menu::sequence::item
