#include "SequenceUse.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

SequenceUse::SequenceUse()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({create.Keyword("use"), create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE), create.Char(';')});
}

void SequenceUse::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);
    auto* definition = state->GetRepository()->GetDataDefinitionByName(typeNameToken.TypeNameValue());
    if (definition == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    auto* definitionWithMembers = dynamic_cast<DefinitionWithMembers*>(definition);
    if (definitionWithMembers == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "Type must be able to have members");

    state->SetInUse(state->GetRepository()->GetInformationFor(definitionWithMembers));
}
