#include "ItemPropertySequences.h"

#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

namespace menu::item_properties
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
            assert(state->m_current_item);

            const auto nameValue = MenuMatcherFactory::TokenTextValue(result.NextCapture(CAPTURE_NAME));
            state->m_current_item->m_name = nameValue;
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
    AddSequence(std::make_unique<SequenceName>());
}
