#include "MenuPropertySequences.h"

#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

namespace menu::menu_properties
{
    class SequenceName final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_NAME = 1;

    public:
        SequenceName()
        {
            const MenuMatcherFactory create(this);

            AddMatchers({
                create.KeywordIgnoreCase("name"),
                create.Text().Capture(CAPTURE_NAME)
            });
        }

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_menu);

            const auto nameValue = MenuMatcherFactory::TokenTextValue(result.NextCapture(CAPTURE_NAME));
            state->m_current_menu->m_name = nameValue;
        }
    };

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
            state->m_current_menu->m_fullscreen = value > 0;
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
    AddSequence(std::make_unique<SequenceName>());
    AddSequence(std::make_unique<SequenceFullScreen>());
}
