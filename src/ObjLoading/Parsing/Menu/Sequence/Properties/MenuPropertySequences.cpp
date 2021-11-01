#include "MenuPropertySequences.h"

#include "GenericBoolPropertySequence.h"
#include "GenericKeywordPropertySequence.h"
#include "GenericStringPropertySequence.h"
#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

namespace menu::menu_properties
{
    class SequenceFullScreen final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_VALUE = 1;

    public:
        SequenceFullScreen()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.KeywordIgnoreCase("fullscreen"),
                create.Numeric().Capture(CAPTURE_VALUE)
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_menu);

            const auto value = MenuMatcherFactory::TokenNumericIntValue(result.NextCapture(CAPTURE_VALUE));
            state->m_current_menu->m_full_screen = value > 0;
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
}
