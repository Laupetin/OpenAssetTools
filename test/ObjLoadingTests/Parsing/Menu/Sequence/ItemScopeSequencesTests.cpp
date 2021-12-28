#include <catch2/catch.hpp>

#include "Utils/ClassUtils.h"
#include "Parsing/Menu/Sequence/ItemScopeSequences.h"
#include "Parsing/Mock/MockLexer.h"

using namespace menu;

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
            SimpleParserValue::EndOfFile(pos)
        });

        helper.m_item->m_feature_type = CommonItemFeatureType::MULTI_VALUE;
        helper.m_item->m_multi_value_features = std::make_unique<CommonItemFeaturesMultiValue>();

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 11);
    }
}
