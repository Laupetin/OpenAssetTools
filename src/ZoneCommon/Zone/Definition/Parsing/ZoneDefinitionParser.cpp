#include "ZoneDefinitionParser.h"

#include "Sequence/SequenceZoneDefinitionAssetList.h"
#include "Sequence/SequenceZoneDefinitionBuild.h"
#include "Sequence/SequenceZoneDefinitionEntry.h"
#include "Sequence/SequenceZoneDefinitionIgnore.h"
#include "Sequence/SequenceZoneDefinitionInclude.h"
#include "Sequence/SequenceZoneDefinitionMetaData.h"

ZoneDefinitionParser::ZoneDefinitionParser(
    ZoneDefinitionLexer* lexer, std::string targetName, ISearchPath& searchPath, IParserLineStream& underlyingStream, const std::optional<GameId> maybeGame)
    : AbstractParser(lexer, std::make_unique<ZoneDefinitionParserState>(std::move(targetName), searchPath, underlyingStream))
{
    if (maybeGame)
        m_state->SetGame(*maybeGame);
}

const std::vector<AbstractParser<ZoneDefinitionParserValue, ZoneDefinitionParserState>::sequence_t*>& ZoneDefinitionParser::GetTestsForState()
{
    static std::vector<sequence_t*> tests({
        new SequenceZoneDefinitionMetaData(),
        new SequenceZoneDefinitionInclude(),
        new SequenceZoneDefinitionIgnore(),
        new SequenceZoneDefinitionAssetList(),
        new SequenceZoneDefinitionBuild(),
        new SequenceZoneDefinitionEntry(),
    });

    return tests;
}

std::unique_ptr<ZoneDefinition> ZoneDefinitionParser::GetParsedValue()
{
    m_state->Finalize();
    return std::move(m_state->m_definition);
}
