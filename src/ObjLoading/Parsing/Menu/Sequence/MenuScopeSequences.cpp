#include "MenuScopeSequences.h"

#include <sstream>

#include "Generic/GenericBoolPropertySequence.h"
#include "Generic/GenericColorPropertySequence.h"
#include "Generic/GenericExpressionPropertySequence.h"
#include "Generic/GenericFloatingPointPropertySequence.h"
#include "Generic/GenericIntPropertySequence.h"
#include "Generic/GenericKeywordPropertySequence.h"
#include "Generic/GenericMenuEventHandlerSetPropertySequence.h"
#include "Generic/GenericStringPropertySequence.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"
#include "Parsing/Menu/Domain/CommonMenuTypes.h"
#include "Parsing/Menu/Matcher/MenuExpressionMatchers.h"

using namespace menu;

namespace menu::menu_scope_sequences
{
    class SequenceCloseBlock final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_TOKEN = 1;

    public:
        SequenceCloseBlock()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.Char('}').Capture(CAPTURE_TOKEN)
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            if (state->m_current_menu->m_name.empty())
                throw ParsingException(result.NextCapture(CAPTURE_TOKEN).GetPos(), "Menu must have a name");

            const auto existingMenu = state->m_menus_by_name.find(state->m_current_menu->m_name);
            if (existingMenu == state->m_menus_by_name.end())
            {
                state->m_menus_by_name.emplace(std::make_pair(state->m_current_menu->m_name, state->m_current_menu));
                state->m_current_menu = nullptr;
            }
            else
            {
                std::ostringstream ss;
                ss << "Menu with name \"" << state->m_current_menu->m_name << "\" already exists";
                throw ParsingException(result.NextCapture(CAPTURE_TOKEN).GetPos(), ss.str());
            }
        }
    };

    class SequenceItemDef final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_TOKEN = 1;

    public:
        SequenceItemDef()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.Keyword("itemDef"),
                create.Char('{')
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            auto newItemDef = std::make_unique<CommonItemDef>();
            state->m_current_item = newItemDef.get();
            state->m_current_menu->m_items.emplace_back(std::move(newItemDef));
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
            assert(state->m_current_menu);

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

            state->m_current_menu->m_rect = rect;
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
                create.String().Capture(CAPTURE_KEY),
                create.Char('{')
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_menu);

            const auto& keyToken = result.NextCapture(CAPTURE_KEY);
            const auto& keyValue = keyToken.StringValue();

            if (keyValue.empty() || keyValue.size() > 1)
                throw ParsingException(keyToken.GetPos(), "Key handler string must have exactly one character");

            const auto key = static_cast<int>(static_cast<unsigned char>(keyValue[0]));

            auto newEventHandlerSet = std::make_unique<CommonEventHandlerSet>();
            state->m_current_event_handler_set = newEventHandlerSet.get();
            state->m_current_nested_event_handler_set = newEventHandlerSet.get();
            state->m_current_menu->m_key_handlers.emplace(std::make_pair(key, std::move(newEventHandlerSet)));
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
                create.Char('{')
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_menu);

            const auto& keyToken = result.NextCapture(CAPTURE_KEY);
            const auto& keyValue = keyToken.IntegerValue();

            if (keyValue < 0)
                throw ParsingException(keyToken.GetPos(), "Key handler value must be positive");

            auto newEventHandlerSet = std::make_unique<CommonEventHandlerSet>();
            state->m_current_event_handler_set = newEventHandlerSet.get();
            state->m_current_nested_event_handler_set = newEventHandlerSet.get();
            state->m_current_menu->m_key_handlers.emplace(std::make_pair(keyValue, std::move(newEventHandlerSet)));
        }
    };
}

using namespace menu_scope_sequences;

MenuScopeSequences::MenuScopeSequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : AbstractScopeSequenceHolder(allSequences, scopeSequences)
{
}

void MenuScopeSequences::AddSequences(FeatureLevel featureLevel)
{
    AddSequence(std::make_unique<SequenceCloseBlock>());
    AddSequence(std::make_unique<SequenceItemDef>());
    AddSequence(std::make_unique<GenericStringPropertySequence>("name", [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
    {
        state->m_current_menu->m_name = value;
    }));
    AddSequence(std::make_unique<GenericBoolPropertySequence>("fullScreen", [](const MenuFileParserState* state, const TokenPos&, const bool value)
    {
        state->m_current_menu->m_full_screen = value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("screenSpace", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_menu->m_screen_space = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("decoration", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_menu->m_decoration = true;
    }));
    AddSequence(std::make_unique<SequenceRect>());
    // rect480
    // rect720
    // pos480
    // pos720
    AddSequence(std::make_unique<GenericIntPropertySequence>("style", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_menu->m_style = value;
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywordAndBool("visible", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ISimpleExpression> value)
    {
        state->m_current_menu->m_visible_expression = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("onOpen", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_menu->m_on_open = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("onClose", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_menu->m_on_close = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("onRequestClose", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_menu->m_on_request_close = std::move(value);
    }));
    AddSequence(std::make_unique<GenericMenuEventHandlerSetPropertySequence>("onESC", [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<CommonEventHandlerSet> value)
    {
        state->m_current_menu->m_on_esc = std::move(value);
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("border", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_menu->m_border = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("borderSize", [](const MenuFileParserState* state, const TokenPos&, const double value)
    {
        state->m_current_menu->m_border_size = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("backcolor", [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
    {
        state->m_current_menu->m_back_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("forecolor", [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
    {
        state->m_current_menu->m_fore_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("bordercolor", [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
    {
        state->m_current_menu->m_border_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("focuscolor", [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
    {
        state->m_current_menu->m_focus_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("outlinecolor", [](const MenuFileParserState* state, const TokenPos&, const CommonColor value)
    {
        state->m_current_menu->m_outline_color = value;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("background", [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
    {
        state->m_current_menu->m_background = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("ownerdraw", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_menu->m_owner_draw = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("ownerdrawFlag", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_menu->m_owner_draw_flags |= value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("outOfBoundsClick", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_menu->m_out_of_bounds_click = true;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("soundLoop", [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
    {
        state->m_current_menu->m_sound_loop = value;
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "X"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ISimpleExpression> value)
    {
        state->m_current_menu->m_rect_x_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "Y"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ISimpleExpression> value)
    {
        state->m_current_menu->m_rect_y_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "W"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ISimpleExpression> value)
    {
        state->m_current_menu->m_rect_w_exp = std::move(value);
    }));
    AddSequence(GenericExpressionPropertySequence::WithKeywords({"exp", "rect", "H"}, [](const MenuFileParserState* state, const TokenPos&, std::unique_ptr<ISimpleExpression> value)
    {
        state->m_current_menu->m_rect_h_exp = std::move(value);
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("popup", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_menu->m_popup = true;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("fadeClamp", [](const MenuFileParserState* state, const TokenPos&, const double value)
    {
        state->m_current_menu->m_fade_clamp = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("fadeCycle", [](const MenuFileParserState* state, const TokenPos&, const int value)
    {
        state->m_current_menu->m_fade_cycle = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("fadeAmount", [](const MenuFileParserState* state, const TokenPos&, const double value)
    {
        state->m_current_menu->m_fade_amount = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("fadeInAmount", [](const MenuFileParserState* state, const TokenPos&, const double value)
    {
        state->m_current_menu->m_fade_in_amount = value;
    }));
    AddSequence(std::make_unique<SequenceExecKey>());
    AddSequence(std::make_unique<SequenceExecKeyInt>());
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("blurWorld", [](const MenuFileParserState* state, const TokenPos&, const double value)
    {
        state->m_current_menu->m_blur_radius = value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("legacySplitScreenScale", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_menu->m_legacy_split_screen_scale = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("hiddenDuringScope", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_menu->m_hidden_during_scope = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("hiddenDuringFlashbang", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_menu->m_hidden_during_flashbang = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("hiddenDuringUI", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_menu->m_hidden_during_ui = true;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("allowedBinding", [](const MenuFileParserState* state, const TokenPos&, const std::string& value)
    {
        state->m_current_menu->m_allowed_binding = value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("textOnlyFocus", [](const MenuFileParserState* state, const TokenPos&)
    {
        state->m_current_menu->m_text_only_focus = true;
    }));
}
