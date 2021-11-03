#include "FunctionPropertySequences.h"

#include "GenericStringPropertySequence.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

using namespace menu;

namespace menu::function_properties
{
}

using namespace function_properties;

FunctionPropertySequences::FunctionPropertySequences(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : AbstractPropertySequenceHolder(allSequences, scopeSequences)
{
}

void FunctionPropertySequences::AddSequences(FeatureLevel featureLevel)
{
    AddSequence(std::make_unique<GenericStringPropertySequence>("name", [](const MenuFileParserState* state, const std::string& value)
    {
        state->m_current_function->m_name = value;
    }));
}
