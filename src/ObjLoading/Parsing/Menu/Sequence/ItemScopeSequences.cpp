#include "ItemScopeSequences.h"

#include "Generic/GenericColorPropertySequence.h"
#include "Generic/GenericExpressionPropertySequence.h"
#include "Generic/GenericFloatingPointPropertySequence.h"
#include "Generic/GenericIntPropertySequence.h"
#include "Generic/GenericKeywordPropertySequence.h"
#include "Generic/GenericMenuEventHandlerSetPropertySequence.h"
#include "Generic/GenericStringPropertySequence.h"
#include "Parsing/Menu/Matcher/MenuExpressionMatchers.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"
#include "Parsing/Menu/MenuFileCommonOperations.h"

#include <string>
#include <vector>

using namespace menu;

class ItemScopeOperations
{
    inline static const CommonItemFeatureType IW4_FEATURE_TYPE_BY_TYPE[0x18]{
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_TEXT
        CommonItemFeatureType::NONE,        // ITEM_TYPE_BUTTON
        CommonItemFeatureType::NONE,        // ITEM_TYPE_RADIOBUTTON
        CommonItemFeatureType::NONE,        // ITEM_TYPE_CHECKBOX
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_EDITFIELD
        CommonItemFeatureType::NONE,        // ITEM_TYPE_COMBO
        CommonItemFeatureType::LISTBOX,     // ITEM_TYPE_LISTBOX
        CommonItemFeatureType::NONE,        // ITEM_TYPE_MODEL
        CommonItemFeatureType::NONE,        // ITEM_TYPE_OWNERDRAW
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_NUMERICFIELD
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_SLIDER
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_YESNO
        CommonItemFeatureType::MULTI_VALUE, // ITEM_TYPE_MULTI
        CommonItemFeatureType::ENUM_DVAR,   // ITEM_TYPE_DVARENUM
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_BIND
        CommonItemFeatureType::NONE,        // ITEM_TYPE_MENUMODEL
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_VALIDFILEFIELD
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_DECIMALFIELD
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_UPREDITFIELD
        CommonItemFeatureType::NONE,        // ITEM_TYPE_GAME_MESSAGE_WINDOW
        CommonItemFeatureType::NEWS_TICKER, // ITEM_TYPE_NEWS_TICKER
        CommonItemFeatureType::NONE,        // ITEM_TYPE_TEXT_SCROLL
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_EMAILFIELD
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_PASSWORDFIELD
    };

    inline static const CommonItemFeatureType IW5_FEATURE_TYPE_BY_TYPE[0x18]{
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_TEXT
        CommonItemFeatureType::NONE,        // ITEM_TYPE_BUTTON
        CommonItemFeatureType::NONE,        // ITEM_TYPE_RADIOBUTTON
        CommonItemFeatureType::NONE,        // ITEM_TYPE_CHECKBOX
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_EDITFIELD
        CommonItemFeatureType::NONE,        // ITEM_TYPE_COMBO
        CommonItemFeatureType::LISTBOX,     // ITEM_TYPE_LISTBOX
        CommonItemFeatureType::NONE,        // ITEM_TYPE_MODEL
        CommonItemFeatureType::NONE,        // ITEM_TYPE_OWNERDRAW
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_NUMERICFIELD
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_SLIDER
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_YESNO
        CommonItemFeatureType::MULTI_VALUE, // ITEM_TYPE_MULTI
        CommonItemFeatureType::ENUM_DVAR,   // ITEM_TYPE_DVARENUM
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_BIND
        CommonItemFeatureType::NONE,        // ITEM_TYPE_MENUMODEL
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_VALIDFILEFIELD
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_DECIMALFIELD
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_UPREDITFIELD
        CommonItemFeatureType::NONE,        // ITEM_TYPE_GAME_MESSAGE_WINDOW
        CommonItemFeatureType::NEWS_TICKER, // ITEM_TYPE_NEWS_TICKER
        CommonItemFeatureType::NONE,        // ITEM_TYPE_TEXT_SCROLL
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_EMAILFIELD
        CommonItemFeatureType::EDIT_FIELD,  // ITEM_TYPE_PASSWORDFIELD
    };

public:
    static void SetItemType(CommonItemDef& item, const FeatureLevel featureLevel, const TokenPos& pos, const int type)
    {
        if (type < 0)
            throw ParsingException(pos, "Invalid item type");

        if (item.m_feature_type != CommonItemFeatureType::NONE)
            throw ParsingException(pos, "Item type has already been set");

        item.m_type = type;

        switch (featureLevel)
        {
        case FeatureLevel::IW4:
            if (static_cast<unsigned>(type) >= std::extent_v<decltype(IW4_FEATURE_TYPE_BY_TYPE)>)
                throw ParsingException(pos, "Invalid item type");
            item.m_feature_type = IW4_FEATURE_TYPE_BY_TYPE[static_cast<unsigned>(type)];
            break;

        case FeatureLevel::IW5:
        default:
            if (static_cast<unsigned>(type) >= std::extent_v<decltype(IW5_FEATURE_TYPE_BY_TYPE)>)
                throw ParsingException(pos, "Invalid item type");
            item.m_feature_type = IW5_FEATURE_TYPE_BY_TYPE[static_cast<unsigned>(type)];
            break;
        }

        switch (item.m_feature_type)
        {
        case CommonItemFeatureType::LISTBOX:
            item.m_list_box_features = std::make_unique<CommonItemFeaturesListBox>();
            break;
        case CommonItemFeatureType::EDIT_FIELD:
            item.m_edit_field_features = std::make_unique<CommonItemFeaturesEditField>();
            break;
        case CommonItemFeatureType::MULTI_VALUE:
            item.m_multi_value_features = std::make_unique<CommonItemFeaturesMultiValue>();
            break;
        case CommonItemFeatureType::NEWS_TICKER:
            item.m_news_ticker_features = std::make_unique<CommonItemFeaturesNewsTicker>();
            break;
        default:
            break;
        }
    }

    static void EnsureHasListboxFeatures(const CommonItemDef& item, const TokenPos& pos)
    {
        if (item.m_feature_type != CommonItemFeatureType::LISTBOX || !item.m_list_box_features)
            throw ParsingException(pos, "Item must have be listbox to use this declaration");
    }

    static void EnsureHasEditFieldFeatures(const CommonItemDef& item, const TokenPos& pos)
    {
        if (item.m_feature_type != CommonItemFeatureType::EDIT_FIELD || !item.m_edit_field_features)
            throw ParsingException(pos, "Item must have be edit field to use this declaration");
    }

    static void EnsureHasMultiValueFeatures(const CommonItemDef& item, const TokenPos& pos)
    {
        if (item.m_feature_type != CommonItemFeatureType::MULTI_VALUE || !item.m_multi_value_features)
            throw ParsingException(pos, "Item must have be multi value to use this declaration");
    }

    static void EnsureHasEnumDvarFeatures(const CommonItemDef& item, const TokenPos& pos)
    {
        if (item.m_feature_type != CommonItemFeatureType::ENUM_DVAR)
            throw ParsingException(pos, "Item must have be enum dvar to use this declaration");
    }

    static void EnsureHasNewsTickerFeatures(const CommonItemDef& item, const TokenPos& pos)
    {
        if (item.m_feature_type != CommonItemFeatureType::NEWS_TICKER || !item.m_news_ticker_features)
            throw ParsingException(pos, "Item must have be news ticker to use this declaration");
    }
};

namespace menu::item_scope_sequences
{
    class SequenceCloseBlock final : public MenuFileParser::sequence_t
    {
    public:
        SequenceCloseBlock()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.Char('}'),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            state->m_current_item = nullptr;
        }
    };

    class SequenceConsumeSemicolons final : public MenuFileParser::sequence_t
    {
    public:
        SequenceConsumeSemicolons()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.Char(';'),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override {}
    };

    class SequenceRect final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_ALIGN_HORIZONTAL = 1;
        static constexpr auto CAPTURE_ALIGN_VERTICAL = 2;

    public:
        SequenceRect()
        {
            const MenuMatcherFactory create(this);

            AddLabeledMatchers(MenuExpressionMatchers().Expression(this), MenuExpressionMatchers::LABEL_EXPRESSION);
            AddMatchers({
                create.KeywordIgnoreCase("rect"),
                create.NumericExpression(), // x
                create.NumericExpression(), // y
                create.NumericExpression(), // w
                create.NumericExpression(), // h
                create.Optional(create.And({
                    create.Integer().Capture(CAPTURE_ALIGN_HORIZONTAL),
                    create.Integer().Capture(CAPTURE_ALIGN_VERTICAL),
                })),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_item);

            const auto x = MenuMatcherFactory::TokenNumericExpressionValue(state, result);
            const auto y = MenuMatcherFactory::TokenNumericExpressionValue(state, result);
            const auto w = MenuMatcherFactory::TokenNumericExpressionValue(state, result);
            const auto h = MenuMatcherFactory::TokenNumericExpressionValue(state, result);
            CommonRect rect{x, y, w, h, 0, 0};

            if (result.HasNextCapture(CAPTURE_ALIGN_HORIZONTAL) && result.HasNextCapture(CAPTURE_ALIGN_VERTICAL))
            {
                rect.horizontalAlign = result.NextCapture(CAPTURE_ALIGN_HORIZONTAL).IntegerValue();
                rect.verticalAlign = result.NextCapture(CAPTURE_ALIGN_VERTICAL).IntegerValue();
            }

            state->m_current_item->m_rect = rect;
        }
    };

    class SequenceOrigin final : public MenuFileParser::sequence_t
    {
    public:
        SequenceOrigin()
        {
            const MenuMatcherFactory create(this);

            AddLabeledMatchers(MenuExpressionMatchers().Expression(this), MenuExpressionMatchers::LABEL_EXPRESSION);
            AddMatchers({
                create.KeywordIgnoreCase("origin"),
                create.NumericExpression(), // x
                create.NumericExpression(), // y
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_item);

            state->m_current_item->m_rect.x = MenuMatcherFactory::TokenNumericExpressionValue(state, result);
            state->m_current_item->m_rect.y = MenuMatcherFactory::TokenNumericExpressionValue(state, result);
        }
    };

    class SequenceDecodeEffect final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_LETTER_TIME = 1;
        static constexpr auto CAPTURE_DECAY_START_TIME = 2;
        static constexpr auto CAPTURE_DECAY_DURATION = 3;

    public:
        SequenceDecodeEffect()
        {
            const MenuMatcherFactory create(this);

            AddLabeledMatchers(MenuExpressionMatchers().Expression(this), MenuExpressionMatchers::LABEL_EXPRESSION);
            AddMatchers({
                create.KeywordIgnoreCase("decodeEffect"),
                create.IntExpression(), // letter time
                create.IntExpression(), // decay start time
                create.IntExpression(), // decay duration
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_item);

            state->m_current_item->m_fx_letter_time = MenuMatcherFactory::TokenIntExpressionValue(state, result);
            state->m_current_item->m_fx_decay_start_time = MenuMatcherFactory::TokenIntExpressionValue(state, result);
            state->m_current_item->m_fx_decay_duration = MenuMatcherFactory::TokenIntExpressionValue(state, result);
        }
    };

    class SequenceMultiTokenBlock final : public MenuFileParser::sequence_t
    {
    public:
        using callback_t = std::function<void(MenuFileParserState* state, const TokenPos& pos, std::vector<std::string> value)>;

    private:
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;
        static constexpr auto CAPTURE_VALUE = 2;

        callback_t m_set_callback;

    public:
        SequenceMultiTokenBlock(std::string keyName, callback_t setCallback)
            : m_set_callback(std::move(setCallback))
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.KeywordIgnoreCase(std::move(keyName)).Capture(CAPTURE_FIRST_TOKEN),
                create.Char('{'),
                create.Optional(create.And({
                    create.Text().Capture(CAPTURE_VALUE),
                    create.OptionalLoop(create.And({
                        create.Char(';'),
                        create.Text().Capture(CAPTURE_VALUE),
                    })),
                })),
                create.Char('}'),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            if (!m_set_callback)
                return;

            std::vector<std::string> values;
            while (result.HasNextCapture(CAPTURE_VALUE))
            {
                values.emplace_back(MenuMatcherFactory::TokenTextValue(result.NextCapture(CAPTURE_VALUE)));
            }

            m_set_callback(state, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), std::move(values));
        }
    };

    class SequenceDvarFloat final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;
        static constexpr auto CAPTURE_DVAR_NAME = 2;

    public:
        SequenceDvarFloat()
        {
            const MenuMatcherFactory create(this);

            AddLabeledMatchers(MenuExpressionMatchers().Expression(this), MenuExpressionMatchers::LABEL_EXPRESSION);
            AddMatchers({
                create.KeywordIgnoreCase("dvarFloat").Capture(CAPTURE_FIRST_TOKEN),
                create.Text().Capture(CAPTURE_DVAR_NAME),
                create.NumericExpression(), // def value
                create.NumericExpression(), // min value
                create.NumericExpression(), // max value
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_item);

            ItemScopeOperations::EnsureHasEditFieldFeatures(*state->m_current_item, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos());
            state->m_current_item->m_dvar = MenuMatcherFactory::TokenTextValue(result.NextCapture(CAPTURE_DVAR_NAME));
            state->m_current_item->m_edit_field_features->m_def_val = MenuMatcherFactory::TokenNumericExpressionValue(state, result);
            state->m_current_item->m_edit_field_features->m_min_val = MenuMatcherFactory::TokenNumericExpressionValue(state, result);
            state->m_current_item->m_edit_field_features->m_max_val = MenuMatcherFactory::TokenNumericExpressionValue(state, result);
        }
    };

    class SequenceDvarStrList final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;
        static constexpr auto CAPTURE_STEP_NAME = 2;
        static constexpr auto CAPTURE_STEP_VALUE = 3;

    public:
        SequenceDvarStrList()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.KeywordIgnoreCase("dvarStrList").Capture(CAPTURE_FIRST_TOKEN),
                create.Char('{'),
                create.OptionalLoop(create.And({
                    create.TextNoChain().Capture(CAPTURE_STEP_NAME),
                    create.Optional(create.Char(';')),
                    create.TextNoChain().Capture(CAPTURE_STEP_VALUE),
                    create.Optional(create.Char(';')),
                })),
                create.Char('}'),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_item);

            ItemScopeOperations::EnsureHasMultiValueFeatures(*state->m_current_item, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos());

            const auto& multiValueFeatures = state->m_current_item->m_multi_value_features;
            while (result.HasNextCapture(CAPTURE_STEP_NAME))
            {
                multiValueFeatures->m_step_names.emplace_back(MenuMatcherFactory::TokenTextValue(result.NextCapture(CAPTURE_STEP_NAME)));
                multiValueFeatures->m_string_values.emplace_back(MenuMatcherFactory::TokenTextValue(result.NextCapture(CAPTURE_STEP_VALUE)));
            }
        }
    };

    class SequenceDvarFloatList final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;
        static constexpr auto CAPTURE_STEP_NAME = 2;

    public:
        SequenceDvarFloatList()
        {
            const MenuMatcherFactory create(this);

            AddLabeledMatchers(MenuExpressionMatchers().Expression(this), MenuExpressionMatchers::LABEL_EXPRESSION);
            AddMatchers({
                create.KeywordIgnoreCase("dvarFloatList").Capture(CAPTURE_FIRST_TOKEN),
                create.Char('{'),
                create.OptionalLoop(create.And({
                    create.Text().Capture(CAPTURE_STEP_NAME),
                    create.Optional(create.Char(';')),
                    create.NumericExpression(),
                    create.Optional(create.Char(';')),
                })),
                create.Char('}'),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_item);

            ItemScopeOperations::EnsureHasMultiValueFeatures(*state->m_current_item, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos());

            const auto& multiValueFeatures = state->m_current_item->m_multi_value_features;
            while (result.HasNextCapture(CAPTURE_STEP_NAME))
            {
                multiValueFeatures->m_step_names.emplace_back(MenuMatcherFactory::TokenTextValue(result.NextCapture(CAPTURE_STEP_NAME)));
                multiValueFeatures->m_double_values.emplace_back(MenuMatcherFactory::TokenNumericExpressionValue(state, result));
            }
        }
    };

    class SequenceColumns final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;
        static constexpr auto CAPTURE_COLUMN_COUNT = 2;
        static constexpr auto CAPTURE_X_POS = 3;
        static constexpr auto CAPTURE_Y_POS = 4;
        static constexpr auto CAPTURE_WIDTH = 5;
        static constexpr auto CAPTURE_HEIGHT = 6;
        static constexpr auto CAPTURE_MAX_CHARS = 7;
        static constexpr auto CAPTURE_ALIGNMENT = 8;

    public:
        explicit SequenceColumns(const FeatureLevel featureLevel)
        {
            const MenuMatcherFactory create(this);

            if (featureLevel == FeatureLevel::IW5)
            {
                AddMatchers({
                    create.KeywordIgnoreCase("columns").Capture(CAPTURE_FIRST_TOKEN),
                    create.Integer().Capture(CAPTURE_COLUMN_COUNT),
                    create.Loop(create.And({
                        create.Integer().Capture(CAPTURE_X_POS),
                        create.Integer().Capture(CAPTURE_Y_POS),
                        create.Integer().Capture(CAPTURE_WIDTH),
                        create.Integer().Capture(CAPTURE_HEIGHT),
                        create.Integer().Capture(CAPTURE_MAX_CHARS),
                        create.Integer().Capture(CAPTURE_ALIGNMENT),
                    })),
                });
            }
            else
            {
                AddMatchers({
                    create.KeywordIgnoreCase("columns").Capture(CAPTURE_FIRST_TOKEN),
                    create.Integer().Capture(CAPTURE_COLUMN_COUNT),
                    create.Loop(create.And({
                        create.Integer().Capture(CAPTURE_X_POS),
                        create.Integer().Capture(CAPTURE_WIDTH),
                        create.Integer().Capture(CAPTURE_MAX_CHARS),
                        create.Integer().Capture(CAPTURE_ALIGNMENT),
                    })),
                });
            }
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_item);

            ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos());

            const auto& listBoxFeatures = state->m_current_item->m_list_box_features;
            while (result.HasNextCapture(CAPTURE_X_POS))
            {
                CommonItemFeaturesListBox::Column column{
                    result.NextCapture(CAPTURE_X_POS).IntegerValue(),
                    state->m_feature_level == FeatureLevel::IW5 ? result.NextCapture(CAPTURE_Y_POS).IntegerValue() : 0,
                    result.NextCapture(CAPTURE_WIDTH).IntegerValue(),
                    state->m_feature_level == FeatureLevel::IW5 ? result.NextCapture(CAPTURE_HEIGHT).IntegerValue() : 0,
                    result.NextCapture(CAPTURE_MAX_CHARS).IntegerValue(),
                    result.NextCapture(CAPTURE_ALIGNMENT).IntegerValue(),
                };
                listBoxFeatures->m_columns.emplace_back(column);
            }
        }
    };

    class SequenceExecKey final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_KEY = 1;

    public:
        SequenceExecKey()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.KeywordIgnoreCase("execKey"),
                create.StringChain().Capture(CAPTURE_KEY),
                create.Char('{'),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_item);

            const auto& keyToken = result.NextCapture(CAPTURE_KEY);
            const auto& keyValue = keyToken.StringValue();

            if (keyValue.empty() || keyValue.size() > 1)
                throw ParsingException(keyToken.GetPos(), "Key handler string must have exactly one character");

            const auto key = static_cast<int>(static_cast<unsigned char>(keyValue[0]));

            auto newEventHandlerSet = std::make_unique<CommonEventHandlerSet>();
            state->m_current_event_handler_set = newEventHandlerSet.get();
            state->m_current_nested_event_handler_set = newEventHandlerSet.get();
            state->m_current_item->m_key_handlers.emplace(std::make_pair(key, std::move(newEventHandlerSet)));
        }
    };

    class SequenceExecKeyInt final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_KEY = 1;

    public:
        SequenceExecKeyInt()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.KeywordIgnoreCase("execKeyInt"),
                create.Integer().Capture(CAPTURE_KEY),
                create.Char('{'),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_item);

            const auto& keyToken = result.NextCapture(CAPTURE_KEY);
            const auto& keyValue = keyToken.IntegerValue();

            if (keyValue < 0)
                throw ParsingException(keyToken.GetPos(), "Key handler value must be positive");

            auto newEventHandlerSet = std::make_unique<CommonEventHandlerSet>();
            state->m_current_event_handler_set = newEventHandlerSet.get();
            state->m_current_nested_event_handler_set = newEventHandlerSet.get();
            state->m_current_item->m_key_handlers.emplace(std::make_pair(keyValue, std::move(newEventHandlerSet)));
        }
    };
} // namespace menu::item_scope_sequences

using namespace item_scope_sequences;

ItemScopeSequences::ItemScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences,
                                       std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void ItemScopeSequences::AddSequences(FeatureLevel featureLevel, bool permissive) const
{
    AddSequence(std::make_unique<SequenceCloseBlock>());
    AddSequence(std::make_unique<GenericStringPropertySequence>("name",
                                                                [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
                                                                {
                                                                    state->m_current_item->m_name = value;
                                                                }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("text",
                                                                [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
                                                                {
                                                                    state->m_current_item->m_text = value;
                                                                }));
    // textfile
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("textsavegame",
                                                                 [](const MenuFileParserState* state, const TokenPos&)
                                                                 {
                                                                     state->m_current_item->m_text_save_game = true;
                                                                 }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("textcinematicsubtitle",
                                                                 [](const MenuFileParserState* state, const TokenPos&)
                                                                 {
                                                                     state->m_current_item->m_text_cinematic_subtitle = true;
                                                                 }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("group",
                                                                [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
                                                                {
                                                                    state->m_current_item->m_group = value;
                                                                }));
    AddSequence(std::make_unique<SequenceRect>());
    AddSequence(std::make_unique<SequenceOrigin>());
    // rect480
    // rect720
    // pos480
    // pos720
    AddSequence(std::make_unique<GenericIntPropertySequence>("style",
                                                             [](const MenuFileParserState* state, const TokenPos&, const int value)
                                                             {
                                                                 state->m_current_item->m_style = value;
                                                             }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("decoration",
                                                                 [](const MenuFileParserState* state, const TokenPos&)
                                                                 {
                                                                     state->m_current_item->m_decoration = true;
                                                                 }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("autowrapped",
                                                                 [](const MenuFileParserState* state, const TokenPos&)
                                                                 {
                                                                     state->m_current_item->m_auto_wrapped = true;
                                                                 }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("horizontalscroll",
                                                                 [](const MenuFileParserState* state, const TokenPos&)
                                                                 {
                                                                     state->m_current_item->m_horizontal_scroll = true;
                                                                 }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("type",
                                                             [](const MenuFileParserState* state, const TokenPos& pos, const int value)
                                                             {
                                                                 ItemScopeOperations::SetItemType(*state->m_current_item, state->m_feature_level, pos, value);
                                                             }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("border",
                                                             [](const MenuFileParserState* state, const TokenPos&, const int value)
                                                             {
                                                                 state->m_current_item->m_border = value;
                                                             }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("borderSize",
                                                                       [](const MenuFileParserState* state, const TokenPos&, const double value)
                                                                       {
                                                                           state->m_current_item->m_border_size = value;
                                                                       }));
    AddSequence(GenericExpressionPropertySequence::WithKeywordAndBool(
        "visible",
        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
        {
            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
            state->m_current_item->m_visible_expression = std::move(value);
        }));
    AddSequence(GenericExpressionPropertySequence::WithKeywordAndBool(
        "disabled",
        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
        {
            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
            state->m_current_item->m_disabled_expression = std::move(value);
        }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("ownerdraw",
                                                             [](const MenuFileParserState* state, const TokenPos&, const int value)
                                                             {
                                                                 state->m_current_item->m_owner_draw = value;
                                                             }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("align",
                                                             [](const MenuFileParserState* state, const TokenPos&, const int value)
                                                             {
                                                                 state->m_current_item->m_align = value;
                                                             }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("textalign",
                                                             [](const MenuFileParserState* state, const TokenPos&, const int value)
                                                             {
                                                                 state->m_current_item->m_text_align = value;
                                                             }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("textalignx",
                                                                       [](const MenuFileParserState* state, const TokenPos&, const double value)
                                                                       {
                                                                           state->m_current_item->m_text_align_x = value;
                                                                       }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("textaligny",
                                                                       [](const MenuFileParserState* state, const TokenPos&, const double value)
                                                                       {
                                                                           state->m_current_item->m_text_align_y = value;
                                                                       }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("textscale",
                                                                       [](const MenuFileParserState* state, const TokenPos&, const double value)
                                                                       {
                                                                           state->m_current_item->m_text_scale = value;
                                                                       }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("textstyle",
                                                             [](const MenuFileParserState* state, const TokenPos&, const int value)
                                                             {
                                                                 state->m_current_item->m_text_style = value;
                                                             }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("textfont",
                                                             [](const MenuFileParserState* state, const TokenPos&, const int value)
                                                             {
                                                                 state->m_current_item->m_text_font = value;
                                                             }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("backcolor",
                                                               [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
                                                               {
                                                                   state->m_current_item->m_back_color = value;
                                                               }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("forecolor",
                                                               [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
                                                               {
                                                                   state->m_current_item->m_fore_color = value;
                                                               }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("bordercolor",
                                                               [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
                                                               {
                                                                   state->m_current_item->m_border_color = value;
                                                               }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("outlinecolor",
                                                               [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
                                                               {
                                                                   state->m_current_item->m_outline_color = value;
                                                               }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("disablecolor",
                                                               [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
                                                               {
                                                                   state->m_current_item->m_disable_color = value;
                                                               }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("glowcolor",
                                                               [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
                                                               {
                                                                   state->m_current_item->m_glow_color = value;
                                                               }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("background",
                                                                [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
                                                                {
                                                                    state->m_current_item->m_background = value;
                                                                }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>(
        "onFocus",
        [](const MenuFileParserState* state, const TokenPos&) -> std::unique_ptr<CommonEventHandlerSet>&
        {
            return state->m_current_item->m_on_focus;
        }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>(
        "leaveFocus",
        [](const MenuFileParserState* state, const TokenPos&) -> std::unique_ptr<CommonEventHandlerSet>&
        {
            return state->m_current_item->m_on_leave_focus;
        }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>(
        "mouseEnter",
        [](const MenuFileParserState* state, const TokenPos&) -> std::unique_ptr<CommonEventHandlerSet>&
        {
            return state->m_current_item->m_on_mouse_enter;
        }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>(
        "mouseExit",
        [](const MenuFileParserState* state, const TokenPos&) -> std::unique_ptr<CommonEventHandlerSet>&
        {
            return state->m_current_item->m_on_mouse_exit;
        }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>(
        "mouseEnterText",
        [](const MenuFileParserState* state, const TokenPos&) -> std::unique_ptr<CommonEventHandlerSet>&
        {
            return state->m_current_item->m_on_mouse_enter_text;
        }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>(
        "mouseExitText",
        [](const MenuFileParserState* state, const TokenPos&) -> std::unique_ptr<CommonEventHandlerSet>&
        {
            return state->m_current_item->m_on_mouse_exit_text;
        }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>(
        "action",
        [](const MenuFileParserState* state, const TokenPos&) -> std::unique_ptr<CommonEventHandlerSet>&
        {
            return state->m_current_item->m_on_action;
        }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>(
        "accept",
        [](const MenuFileParserState* state, const TokenPos&) -> std::unique_ptr<CommonEventHandlerSet>&
        {
            return state->m_current_item->m_on_accept;
        }));
    // special
    AddSequence(std::make_unique<GenericStringPropertySequence>("dvar",
                                                                [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
                                                                {
                                                                    state->m_current_item->m_dvar = value;
                                                                }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("focusSound",
                                                                [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
                                                                {
                                                                    state->m_current_item->m_focus_sound = value;
                                                                }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("ownerdrawFlag",
                                                             [](const MenuFileParserState* state, const TokenPos&, const int value)
                                                             {
                                                                 state->m_current_item->m_owner_draw_flags |= value;
                                                             }));
    AddSequence(std::make_unique<SequenceMultiTokenBlock>("enableDvar",
                                                          [](const MenuFileParserState* state, const TokenPos&, std::vector<std::string> value)
                                                          {
                                                              state->m_current_item->m_enable_dvar = std::move(value);
                                                          }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("dvarTest",
                                                                [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
                                                                {
                                                                    state->m_current_item->m_dvar_test = value;
                                                                }));
    AddSequence(std::make_unique<SequenceMultiTokenBlock>("disableDvar",
                                                          [](const MenuFileParserState* state, const TokenPos&, std::vector<std::string> value)
                                                          {
                                                              state->m_current_item->m_disable_dvar = std::move(value);
                                                          }));
    AddSequence(std::make_unique<SequenceMultiTokenBlock>("showDvar",
                                                          [](const MenuFileParserState* state, const TokenPos&, std::vector<std::string> value)
                                                          {
                                                              state->m_current_item->m_show_dvar = std::move(value);
                                                          }));
    AddSequence(std::make_unique<SequenceMultiTokenBlock>("hideDvar",
                                                          [](const MenuFileParserState* state, const TokenPos&, std::vector<std::string> value)
                                                          {
                                                              state->m_current_item->m_hide_dvar = std::move(value);
                                                          }));
    AddSequence(std::make_unique<SequenceMultiTokenBlock>("focusDvar",
                                                          [](const MenuFileParserState* state, const TokenPos&, std::vector<std::string> value)
                                                          {
                                                              state->m_current_item->m_focus_dvar = std::move(value);
                                                          }));
    AddSequence(std::make_unique<SequenceExecKey>());
    AddSequence(std::make_unique<SequenceExecKeyInt>());
    AddSequence(std::make_unique<GenericIntPropertySequence>("gamemsgwindowindex",
                                                             [](const MenuFileParserState* state, const TokenPos&, const int value)
                                                             {
                                                                 state->m_current_item->m_game_message_window_index = value;
                                                             }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("gamemsgwindowmode",
                                                             [](const MenuFileParserState* state, const TokenPos&, const int value)
                                                             {
                                                                 state->m_current_item->m_game_message_window_mode = value;
                                                             }));
    AddSequence(std::make_unique<SequenceDecodeEffect>());
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "disabled"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_disabled_expression = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "text"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsStringExpression(state, pos, *value);
                                                            state->m_current_item->m_text_expression = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "material"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsStringExpression(state, pos, *value);
                                                            state->m_current_item->m_material_expression = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "X"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_rect_x_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "Y"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_rect_y_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "W"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_rect_w_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "H"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_rect_h_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "forecolor", "R"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_forecolor_expressions.m_r_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "forecolor", "G"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_forecolor_expressions.m_g_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "forecolor", "B"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_forecolor_expressions.m_b_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "forecolor", "A"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_forecolor_expressions.m_a_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "forecolor", "RGB"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_forecolor_expressions.m_rgb_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "glowcolor", "R"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_glowcolor_expressions.m_r_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "glowcolor", "G"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_glowcolor_expressions.m_g_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "glowcolor", "B"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_glowcolor_expressions.m_b_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "glowcolor", "A"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_glowcolor_expressions.m_a_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "glowcolor", "RGB"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_glowcolor_expressions.m_rgb_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "backcolor", "R"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_backcolor_expressions.m_r_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "backcolor", "G"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_backcolor_expressions.m_g_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "backcolor", "B"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_backcolor_expressions.m_b_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "backcolor", "A"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_backcolor_expressions.m_a_exp = std::move(value);
                                                        }));
    AddSequence(
        GenericExpressionPropertySequence::WithKeywords({"exp", "backcolor", "RGB"},
                                                        [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                        {
                                                            MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                            state->m_current_item->m_backcolor_expressions.m_rgb_exp = std::move(value);
                                                        }));

    if (featureLevel == FeatureLevel::IW5)
    {
        AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>(
            "hasFocus",
            [](const MenuFileParserState* state, const TokenPos&) -> std::unique_ptr<CommonEventHandlerSet>&
            {
                return state->m_current_item->m_has_focus;
            }));
        AddSequence(
            GenericExpressionPropertySequence::WithKeywords({"exp", "textaligny"},
                                                            [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                            {
                                                                MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                                state->m_current_item->m_text_align_y_expression = std::move(value);
                                                            }));
    }

    // ============== ListBox ==============
    AddSequence(std::make_unique<SequenceColumns>(featureLevel));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("notselectable",
                                                                 [](const MenuFileParserState* state, const TokenPos& pos)
                                                                 {
                                                                     ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, pos);
                                                                     state->m_current_item->m_list_box_features->m_not_selectable = true;
                                                                 }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("noscrollbars",
                                                                 [](const MenuFileParserState* state, const TokenPos& pos)
                                                                 {
                                                                     ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, pos);
                                                                     state->m_current_item->m_list_box_features->m_no_scrollbars = true;
                                                                 }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("usepaging",
                                                                 [](const MenuFileParserState* state, const TokenPos& pos)
                                                                 {
                                                                     ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, pos);
                                                                     state->m_current_item->m_list_box_features->m_use_paging = true;
                                                                 }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("elementwidth",
                                                                       [](const MenuFileParserState* state, const TokenPos& pos, const double value)
                                                                       {
                                                                           ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, pos);
                                                                           state->m_current_item->m_list_box_features->m_element_width = value;
                                                                       }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("elementheight",
                                                                       [](const MenuFileParserState* state, const TokenPos& pos, const double value)
                                                                       {
                                                                           ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, pos);
                                                                           state->m_current_item->m_list_box_features->m_element_height = value;
                                                                       }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("feeder",
                                                                       [](const MenuFileParserState* state, const TokenPos& pos, const double value)
                                                                       {
                                                                           ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, pos);
                                                                           state->m_current_item->m_list_box_features->m_feeder = value;
                                                                       }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("elementtype",
                                                             [](const MenuFileParserState* state, const TokenPos& pos, const int value)
                                                             {
                                                                 ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, pos);
                                                                 state->m_current_item->m_list_box_features->m_element_style = value;
                                                             }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>(
        "doubleclick",
        [](const MenuFileParserState* state, const TokenPos& pos) -> std::unique_ptr<CommonEventHandlerSet>&
        {
            ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, pos);
            return state->m_current_item->m_list_box_features->m_on_double_click;
        }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("selectBorder",
                                                               [](const MenuFileParserState* state, const TokenPos& pos, const CommonColor value)
                                                               {
                                                                   ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, pos);
                                                                   state->m_current_item->m_list_box_features->m_select_border = value;
                                                               }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("selectIcon",
                                                                [](const MenuFileParserState* state, const TokenPos& pos, const std::string& value)
                                                                {
                                                                    ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, pos);
                                                                    state->m_current_item->m_list_box_features->m_select_icon = value;
                                                                }));

    if (featureLevel == FeatureLevel::IW5)
    {
        AddSequence(
            GenericExpressionPropertySequence::WithKeywords({"exp", "elementheight"},
                                                            [](const MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ISimpleExpression> value)
                                                            {
                                                                ItemScopeOperations::EnsureHasListboxFeatures(*state->m_current_item, pos);
                                                                MenuFileCommonOperations::EnsureIsNumericExpression(state, pos, *value);
                                                                state->m_current_item->m_list_box_features->m_element_height_expression = std::move(value);
                                                            }));
    }

    // ============== Edit Field ==============
    AddSequence(std::make_unique<SequenceDvarFloat>());
    AddSequence(std::make_unique<GenericStringPropertySequence>("localvar",
                                                                [](const MenuFileParserState* state, const TokenPos& pos, const std::string& value)
                                                                {
                                                                    ItemScopeOperations::EnsureHasEditFieldFeatures(*state->m_current_item, pos);
                                                                    state->m_current_item->m_edit_field_features->m_local_var = value;
                                                                }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("maxChars",
                                                             [](const MenuFileParserState* state, const TokenPos& pos, const int value)
                                                             {
                                                                 ItemScopeOperations::EnsureHasEditFieldFeatures(*state->m_current_item, pos);
                                                                 state->m_current_item->m_edit_field_features->m_max_chars = value;
                                                             }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("maxPaintChars",
                                                             [](const MenuFileParserState* state, const TokenPos& pos, const int value)
                                                             {
                                                                 ItemScopeOperations::EnsureHasEditFieldFeatures(*state->m_current_item, pos);
                                                                 state->m_current_item->m_edit_field_features->m_max_paint_chars = value;
                                                             }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("maxCharsGotoNext",
                                                                 [](const MenuFileParserState* state, const TokenPos& pos)
                                                                 {
                                                                     ItemScopeOperations::EnsureHasEditFieldFeatures(*state->m_current_item, pos);
                                                                     state->m_current_item->m_edit_field_features->m_max_chars_goto_next = true;
                                                                 }));

    // ============== Multi Value ==============
    AddSequence(std::make_unique<SequenceDvarStrList>());
    AddSequence(std::make_unique<SequenceDvarFloatList>());

    // ============== Enum Dvar ==============
    AddSequence(std::make_unique<GenericStringPropertySequence>("dvarEnumList",
                                                                [](const MenuFileParserState* state, const TokenPos& pos, const std::string& value)
                                                                {
                                                                    ItemScopeOperations::EnsureHasEnumDvarFeatures(*state->m_current_item, pos);
                                                                    state->m_current_item->m_enum_dvar_name = value;
                                                                }));

    // ============== News Ticker ==============
    AddSequence(std::make_unique<GenericIntPropertySequence>("spacing",
                                                             [](const MenuFileParserState* state, const TokenPos& pos, const int value)
                                                             {
                                                                 ItemScopeOperations::EnsureHasNewsTickerFeatures(*state->m_current_item, pos);
                                                                 state->m_current_item->m_news_ticker_features->m_spacing = value;
                                                             }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("speed",
                                                             [](const MenuFileParserState* state, const TokenPos& pos, const int value)
                                                             {
                                                                 ItemScopeOperations::EnsureHasNewsTickerFeatures(*state->m_current_item, pos);
                                                                 state->m_current_item->m_news_ticker_features->m_speed = value;
                                                             }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("newsfeed",
                                                             [](const MenuFileParserState* state, const TokenPos& pos, const int value)
                                                             {
                                                                 ItemScopeOperations::EnsureHasNewsTickerFeatures(*state->m_current_item, pos);
                                                                 state->m_current_item->m_news_ticker_features->m_news_feed_id = value;
                                                             }));

    AddSequence(std::make_unique<SequenceConsumeSemicolons>());
}
