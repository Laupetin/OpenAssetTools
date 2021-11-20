#include "ItemScopeSequences.h"

#include <vector>
#include <string>

#include "Generic/GenericColorPropertySequence.h"
#include "Generic/GenericExpressionPropertySequence.h"
#include "Generic/GenericFloatingPointPropertySequence.h"
#include "Generic/GenericIntPropertySequence.h"
#include "Generic/GenericKeywordPropertySequence.h"
#include "Generic/GenericMenuEventHandlerSetPropertySequence.h"
#include "Generic/GenericStringPropertySequence.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

using namespace menu;

namespace menu::item_scope_sequences
{
    class SequenceCloseBlock final : public MenuFileParser::sequence_t
    {
    public:
        SequenceCloseBlock()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.Char('}')
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            state->m_current_item = nullptr;
        }
    };

    class SequenceRect final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_X = 1;
        static constexpr auto CAPTURE_Y = 2;
        static constexpr auto CAPTURE_W = 3;
        static constexpr auto CAPTURE_H = 4;
        static constexpr auto CAPTURE_ALIGN_HORIZONTAL = 5;
        static constexpr auto CAPTURE_ALIGN_VERTICAL = 6;

    public:
        SequenceRect()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.KeywordIgnoreCase("rect"),
                create.Numeric().Capture(CAPTURE_X),
                create.Numeric().Capture(CAPTURE_Y),
                create.Numeric().Capture(CAPTURE_W),
                create.Numeric().Capture(CAPTURE_H),
                create.Optional(create.And({
                    create.Integer().Capture(CAPTURE_ALIGN_HORIZONTAL),
                    create.Integer().Capture(CAPTURE_ALIGN_VERTICAL)
                }))
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_item);

            CommonRect rect
            {
                MenuMatcherFactory::TokenNumericFloatingPointValue(result.NextCapture(CAPTURE_X)),
                MenuMatcherFactory::TokenNumericFloatingPointValue(result.NextCapture(CAPTURE_Y)),
                MenuMatcherFactory::TokenNumericFloatingPointValue(result.NextCapture(CAPTURE_W)),
                MenuMatcherFactory::TokenNumericFloatingPointValue(result.NextCapture(CAPTURE_H)),
                0, 0
            };

            if (result.HasNextCapture(CAPTURE_ALIGN_HORIZONTAL) && result.HasNextCapture(CAPTURE_ALIGN_VERTICAL))
            {
                rect.horizontalAlign = result.NextCapture(CAPTURE_ALIGN_HORIZONTAL).IntegerValue();
                rect.verticalAlign = result.NextCapture(CAPTURE_ALIGN_VERTICAL).IntegerValue();
            }

            state->m_current_item->m_rect = rect;
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

            AddMatchers({
                create.KeywordIgnoreCase("decodeEffect"),
                create.Integer().Capture(CAPTURE_LETTER_TIME),
                create.Integer().Capture(CAPTURE_DECAY_START_TIME),
                create.Integer().Capture(CAPTURE_DECAY_DURATION),
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_item);

            state->m_current_item->m_fx_letter_time = result.NextCapture(CAPTURE_LETTER_TIME).IntegerValue();
            state->m_current_item->m_fx_decay_start_time = result.NextCapture(CAPTURE_DECAY_START_TIME).IntegerValue();
            state->m_current_item->m_fx_decay_duration = result.NextCapture(CAPTURE_DECAY_DURATION).IntegerValue();
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
                        create.Text().Capture(CAPTURE_VALUE)
                    }))
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
}

using namespace item_scope_sequences;

class ItemScopeOperations
{
    inline static const CommonItemFeatureType IW4_FEATURE_TYPE_BY_TYPE[0x18]
    {
        CommonItemFeatureType::EDIT_FIELD, // ITEM_TYPE_TEXT
        CommonItemFeatureType::NONE, // ITEM_TYPE_BUTTON
        CommonItemFeatureType::NONE, // ITEM_TYPE_RADIOBUTTON
        CommonItemFeatureType::NONE, // ITEM_TYPE_CHECKBOX
        CommonItemFeatureType::EDIT_FIELD, // ITEM_TYPE_EDITFIELD
        CommonItemFeatureType::NONE, // ITEM_TYPE_COMBO
        CommonItemFeatureType::LISTBOX, // ITEM_TYPE_LISTBOX
        CommonItemFeatureType::NONE, // ITEM_TYPE_MODEL
        CommonItemFeatureType::NONE, // ITEM_TYPE_OWNERDRAW
        CommonItemFeatureType::EDIT_FIELD, // ITEM_TYPE_NUMERICFIELD
        CommonItemFeatureType::EDIT_FIELD, // ITEM_TYPE_SLIDER
        CommonItemFeatureType::EDIT_FIELD, // ITEM_TYPE_YESNO
        CommonItemFeatureType::MULTI_VALUE, // ITEM_TYPE_MULTI
        CommonItemFeatureType::ENUM_DVAR, // ITEM_TYPE_DVARENUM
        CommonItemFeatureType::EDIT_FIELD, // ITEM_TYPE_BIND
        CommonItemFeatureType::NONE, // ITEM_TYPE_MENUMODEL
        CommonItemFeatureType::EDIT_FIELD, // ITEM_TYPE_VALIDFILEFIELD
        CommonItemFeatureType::EDIT_FIELD, // ITEM_TYPE_DECIMALFIELD
        CommonItemFeatureType::EDIT_FIELD, // ITEM_TYPE_UPREDITFIELD
        CommonItemFeatureType::NONE, // ITEM_TYPE_GAME_MESSAGE_WINDOW
        CommonItemFeatureType::NEWS_TICKER, // ITEM_TYPE_NEWS_TICKER
        CommonItemFeatureType::NONE, // ITEM_TYPE_TEXT_SCROLL
        CommonItemFeatureType::EDIT_FIELD, // ITEM_TYPE_EMAILFIELD
        CommonItemFeatureType::EDIT_FIELD // ITEM_TYPE_PASSWORDFIELD
    };

public:
    static void SetItemType(CommonItemDef& item, const FeatureLevel featureLevel, const TokenPos& pos, const int type)
    {
        if (type < 0)
            throw ParsingException(pos, "Invalid item type");

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
            assert(false);
            throw ParsingException(pos, "Unimplemented item types for feature level");
        }
    }
};

ItemScopeSequences::ItemScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void ItemScopeSequences::AddSequences(FeatureLevel featureLevel)
{
    AddSequence(std::make_unique<SequenceCloseBlock>());
    AddSequence(std::make_unique<GenericStringPropertySequence>("name", [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
    {
        state->m_current_item->m_name = value;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("text", [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
    {
        state->m_current_item->m_text = value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("textsavegame", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_item->m_text_save_game = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("textcinematicsubtitle", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_item->m_text_cinematic_subtitle = true;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("group", [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
    {
        state->m_current_item->m_group = value;
    }));
    AddSequence(std::make_unique<SequenceRect>());
    AddSequence(std::make_unique<GenericIntPropertySequence>("style", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_item->m_style = value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("decoration", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_item->m_decoration = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("autowrapped", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_item->m_auto_wrapped = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("horizontalscroll", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_item->m_horizontal_scroll = true;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("type", [](const MenuFileParserState* state, const TokenPos& pos, const int value)
    {
        ItemScopeOperations::SetItemType(*state->m_current_item, state->m_feature_level, pos, value);
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("border", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_item->m_border = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("borderSize", [](const MenuFileParserState* state, const TokenPos&, const double value)
    {
        state->m_current_item->m_border_size = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("ownerdraw", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_item->m_owner_draw = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("align", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_item->m_align = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("textalign", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_item->m_text_align = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("textalignx", [](const MenuFileParserState* state, const TokenPos&, const double value)
    {
        state->m_current_item->m_text_align_x = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("textaligny", [](const MenuFileParserState* state, const TokenPos&, const double value)
    {
        state->m_current_item->m_text_align_y = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("textscale", [](const MenuFileParserState* state, const TokenPos&, const double value)
    {
        state->m_current_item->m_text_scale = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("textstyle", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_item->m_text_style = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("textfont", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_item->m_text_font = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("backcolor", [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
    {
        state->m_current_item->m_back_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("forecolor", [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
    {
        state->m_current_item->m_fore_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("bordercolor", [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
    {
        state->m_current_item->m_border_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("outlinecolor", [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
    {
        state->m_current_item->m_outline_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("disablecolor", [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
    {
        state->m_current_item->m_disable_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("glowcolor", [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
    {
        state->m_current_item->m_glow_color = value;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("background", [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
    {
        state->m_current_item->m_background = value;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("focusSound", [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
    {
        state->m_current_item->m_focus_sound = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("ownerdrawFlag", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_item->m_owner_draw_flags |= value;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("dvarTest", [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
    {
        state->m_current_item->m_dvar_test = value;
    }));
    AddSequence(std::make_unique<SequenceMultiTokenBlock>("enableDvar", [](const MenuFileParserState* state, const TokenPos&, std::vector<std::string> value)
    {
        state->m_current_item->m_enable_dvar = std::move(value);
    }));
    AddSequence(std::make_unique<SequenceMultiTokenBlock>("disableDvar", [](const MenuFileParserState* state, const TokenPos&, std::vector<std::string> value)
    {
        state->m_current_item->m_disable_dvar = std::move(value);
    }));
    AddSequence(std::make_unique<SequenceMultiTokenBlock>("showDvar", [](const MenuFileParserState* state, const TokenPos&, std::vector<std::string> value)
    {
        state->m_current_item->m_show_dvar = std::move(value);
    }));
    AddSequence(std::make_unique<SequenceMultiTokenBlock>("hideDvar", [](const MenuFileParserState* state, const TokenPos&, std::vector<std::string> value)
    {
        state->m_current_item->m_hide_dvar = std::move(value);
    }));
    AddSequence(std::make_unique<SequenceMultiTokenBlock>("focusDvar", [](const MenuFileParserState* state, const TokenPos&, std::vector<std::string> value)
    {
        state->m_current_item->m_focus_dvar = std::move(value);
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("gamemsgwindowindex", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_item->m_game_message_window_index = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("gamemsgwindowmode", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_item->m_game_message_window_mode = value;
    }));
    AddSequence(std::make_unique<SequenceDecodeEffect>());
    AddSequence(GenericExpressionPropertySequence::WithKeywordAndBool("visible", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_visible_expression = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywordAndBool("disabled", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_disabled_expression = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "disabled"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_disabled_expression = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "text"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_text_expression = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "material"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_material_expression = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "material"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_material_expression = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "X"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_rect_x_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "Y"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_rect_y_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "W"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_rect_w_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "H"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_rect_h_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "forecolor", "R"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_forecolor_expressions.m_r_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "forecolor", "G"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_forecolor_expressions.m_g_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "forecolor", "B"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_forecolor_expressions.m_b_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "forecolor", "A"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_forecolor_expressions.m_a_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "forecolor", "RGB"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_forecolor_expressions.m_rgb_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "glowcolor", "R"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_glowcolor_expressions.m_r_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "glowcolor", "G"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_glowcolor_expressions.m_g_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "glowcolor", "B"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_glowcolor_expressions.m_b_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "glowcolor", "A"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_glowcolor_expressions.m_a_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "glowcolor", "RGB"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_glowcolor_expressions.m_rgb_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "backcolor", "R"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_backcolor_expressions.m_r_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "backcolor", "G"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_backcolor_expressions.m_g_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "backcolor", "B"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_backcolor_expressions.m_b_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "backcolor", "A"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_backcolor_expressions.m_a_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "backcolor", "RGB"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ICommonExpression> value)
    {
        state->m_current_item->m_backcolor_expressions.m_rgb_exp = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("onFocus", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_item->m_on_focus = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("leaveFocus", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_item->m_on_leave_focus = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("mouseEnter", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_item->m_on_mouse_enter = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("mouseExit", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_item->m_on_mouse_exit = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("mouseEnterText", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_item->m_on_mouse_enter_text = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("mouseExitText", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_item->m_on_mouse_exit_text = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("action", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_item->m_on_action = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("accept", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_item->m_on_accept = std::move(value);
    }));
}
