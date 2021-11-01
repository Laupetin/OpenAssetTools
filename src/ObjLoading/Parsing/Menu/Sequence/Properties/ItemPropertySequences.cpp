#include "ItemPropertySequences.h"

#include "GenericColorPropertySequence.h"
#include "GenericFloatingPointPropertySequence.h"
#include "GenericIntPropertySequence.h"
#include "GenericKeywordPropertySequence.h"
#include "GenericStringPropertySequence.h"
#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

namespace menu::item_properties
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
}

using namespace item_properties;

ItemPropertySequences::ItemPropertySequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : AbstractPropertySequenceHolder(allSequences, scopeSequences)
{
}

void ItemPropertySequences::AddSequences(FeatureLevel featureLevel)
{
    AddSequence(std::make_unique<GenericStringPropertySequence>("name", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_item->m_name = value;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("text", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_item->m_text = value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("textsavegame", [](const MenuFileParserState* state)
    {
        state->m_current_item->m_text_save_game = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("textcinematicsubtitle", [](const MenuFileParserState* state)
    {
        state->m_current_item->m_text_cinematic_subtitle = true;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("group", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_item->m_group = value;
    }));
    AddSequence(std::make_unique<SequenceRect>());
    AddSequence(std::make_unique<GenericIntPropertySequence>("style", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_item->m_style = value;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("decoration", [](const MenuFileParserState* state)
    {
        state->m_current_item->m_decoration = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("autowrapped", [](const MenuFileParserState* state)
    {
        state->m_current_item->m_auto_wrapped = true;
    }));
    AddSequence(std::make_unique<GenericKeywordPropertySequence>("horizontalscroll", [](const MenuFileParserState* state)
    {
        state->m_current_item->m_horizontal_scroll = true;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("type", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_item->m_type = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("border", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_item->m_border = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("borderSize", [](const MenuFileParserState* state, const double value)
    {
        state->m_current_item->m_border_size = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("ownerdraw", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_item->m_owner_draw = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("align", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_item->m_align = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("textalign", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_item->m_text_align = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("textalignx", [](const MenuFileParserState* state, const double value)
    {
        state->m_current_item->m_text_align_x = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("textaligny", [](const MenuFileParserState* state, const double value)
    {
        state->m_current_item->m_text_align_y = value;
    }));
    AddSequence(std::make_unique<GenericFloatingPointPropertySequence>("textscale", [](const MenuFileParserState* state, const double value)
    {
        state->m_current_item->m_text_scale = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("textstyle", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_item->m_text_style = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("textfont", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_item->m_text_font = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("backcolor", [](const MenuFileParserState* state, const CommonColor value)
    {
        state->m_current_item->m_back_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("forecolor", [](const MenuFileParserState* state, const CommonColor value)
    {
        state->m_current_item->m_fore_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("bordercolor", [](const MenuFileParserState* state, const CommonColor value)
    {
        state->m_current_item->m_border_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("outlinecolor", [](const MenuFileParserState* state, const CommonColor value)
    {
        state->m_current_item->m_outline_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("disablecolor", [](const MenuFileParserState* state, const CommonColor value)
    {
        state->m_current_item->m_disable_color = value;
    }));
    AddSequence(std::make_unique<GenericColorPropertySequence>("glowcolor", [](const MenuFileParserState* state, const CommonColor value)
    {
        state->m_current_item->m_glow_color = value;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("background", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_item->m_background = value;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("focusSound", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_item->m_focus_sound = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("ownerdrawFlag", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_item->m_owner_draw_flags |= value;
    }));
    AddSequence(std::make_unique<GenericStringPropertySequence>("dvarTest", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_item->m_dvar_test = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("gamemsgwindowindex", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_item->m_game_message_window_index = value;
    }));
    AddSequence(std::make_unique<GenericIntPropertySequence>("gamemsgwindowmode", [](const MenuFileParserState* state, const int value)
    {
        state->m_current_item->m_game_message_window_mode = value;
    }));
    AddSequence(std::make_unique<SequenceDecodeEffect>());
}
