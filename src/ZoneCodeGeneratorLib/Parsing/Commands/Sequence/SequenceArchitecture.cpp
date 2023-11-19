#include "SequenceArchitecture.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

SequenceArchitecture::SequenceArchitecture()
{
    const CommandsMatcherFactory create(this);

    AddMatchers({create.Keyword("architecture"), create.Identifier().Capture(CAPTURE_ARCHITECTURE), create.Char(';')});

    m_architecture_mapping["x86"] = Architecture::X86;
    m_architecture_mapping["x64"] = Architecture::X64;
}

void SequenceArchitecture::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& architectureToken = result.NextCapture(CAPTURE_ARCHITECTURE);

    const auto foundArchitecture = m_architecture_mapping.find(architectureToken.IdentifierValue());

    if (foundArchitecture == m_architecture_mapping.end())
        throw ParsingException(architectureToken.GetPos(), "Unknown architecture");

    state->SetArchitecture(foundArchitecture->second);
}
