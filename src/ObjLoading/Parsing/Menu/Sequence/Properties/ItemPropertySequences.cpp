#include "ItemPropertySequences.h"

#include "GenericStringPropertySequence.h"
#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

namespace menu::item_properties
{
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
}
