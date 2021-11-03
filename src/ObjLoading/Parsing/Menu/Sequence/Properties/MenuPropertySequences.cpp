#include "MenuPropertySequences.h"

#include "GenericBoolPropertySequence.h"
#include "GenericColorPropertySequence.h"
#include "GenericFloatingPointPropertySequence.h"
#include "GenericIntPropertySequence.h"
#include "GenericKeywordPropertySequence.h"
#include "GenericStringPropertySequence.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"
#include "Parsing/Menu/Domain/CommonMenuTypes.h"

using namespace menu;

namespace menu::menu_properties
{
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
}

using namespace menu_properties;

MenuPropertySequences::MenuPropertySequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : AbstractPropertySequenceHolder(allSequences, scopeSequences)
{
}

void MenuPropertySequences::AddSequences(FeatureLevel featureLevel)
{
    AddSequence(std::make_unique<GenericStringPropertySequence>("name", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_menu->m_name = value;
    }));
    AddSequence(std::make_unique<GenericBoolPropertySequence>("fullScreen", [](const MenuFileParserState* state, const bool value)
    {
        state->m_current_menu->m_full_screen = value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("screenSpace", [](const MenuFileParserState* state)
    {
        state->m_current_menu->m_screen_space = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("decoration", [](const MenuFileParserState* state)
    {
        state->m_current_menu->m_decoration = true;
    }));
    AddSequence(std::make_unique<SequenceRect>());
    AddSequence(std::make_unique<GenericIntPropertySequence>("style", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_menu->m_style = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("border", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_menu->m_border = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("borderSize", [](const MenuFileParserState* state, const double value)
    {
        state->m_current_menu->m_border_size = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("backcolor", [](const MenuFileParserState* state, const CommonColor value)
    {
        state->m_current_menu->m_back_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("forecolor", [](const MenuFileParserState* state, const CommonColor value)
    {
        state->m_current_menu->m_fore_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("bordercolor", [](const MenuFileParserState* state, const CommonColor value)
    {
        state->m_current_menu->m_border_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("focuscolor", [](const MenuFileParserState* state, const CommonColor value)
    {
        state->m_current_menu->m_focus_color = value;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("background", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_menu->m_background = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("ownerdraw", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_menu->m_owner_draw = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("ownerdrawFlag", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_menu->m_owner_draw_flags |= value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("outOfBoundsClick", [](const MenuFileParserState* state)
    {
        state->m_current_menu->m_out_of_bounds_click = true;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("soundLoop", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_menu->m_sound_loop = value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("popup", [](const MenuFileParserState* state)
    {
        state->m_current_menu->m_popup = true;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("fadeClamp", [](const MenuFileParserState* state, const double value)
    {
        state->m_current_menu->m_fade_clamp = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("fadeCycle", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_menu->m_fade_cycle = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("fadeAmount", [](const MenuFileParserState* state, const double value)
    {
        state->m_current_menu->m_fade_amount = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("fadeInAmount", [](const MenuFileParserState* state, const double value)
    {
        state->m_current_menu->m_fade_in_amount = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("blurWorld", [](const MenuFileParserState* state, const double value)
    {
        state->m_current_menu->m_blur_radius = value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("legacySplitScreenScale", [](const MenuFileParserState* state)
    {
        state->m_current_menu->m_legacy_split_screen_scale = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("hiddenDuringScope", [](const MenuFileParserState* state)
    {
        state->m_current_menu->m_hidden_during_scope = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("hiddenDuringFlashbang", [](const MenuFileParserState* state)
    {
        state->m_current_menu->m_hidden_during_flashbang = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("hiddenDuringUI", [](const MenuFileParserState* state)
    {
        state->m_current_menu->m_hidden_during_ui = true;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("allowedBinding", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_menu->m_allowed_binding = value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("textOnlyFocus", [](const MenuFileParserState* state)
    {
        state->m_current_menu->m_text_only_focus = true;
    }));
}
