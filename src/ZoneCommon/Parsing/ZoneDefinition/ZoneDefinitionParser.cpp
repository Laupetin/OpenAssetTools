#include "ZoneDefinitionParser.h"

#include "Sequence/SequenceZoneDefinitionEntry.h"
#include "Sequence/SequenceZoneDefinitionIgnore.h"
#include "Sequence/SequenceZoneDefinitionInclude.h"
#include "Sequence/SequenceZoneDefinitionMetaData.h"

ZoneDefinitionParser::ZoneDefinitionParser(SimpleLexer* lexer)
    : AbstractParser(lexer, std::make_unique<ZoneDefinition>())
{
}

const std::vector<AbstractParser<SimpleParserValue, ZoneDefinition>::sequence_t*>& ZoneDefinitionParser::GetTestsForState()
{
    static std::vector<sequence_t*> tests({
        new SequenceZoneDefinitionMetaData(),
        new SequenceZoneDefinitionInclude(),
        new SequenceZoneDefinitionIgnore(),
        new SequenceZoneDefinitionEntry()
    });

    return tests;
}

std::unique_ptr<ZoneDefinition> ZoneDefinitionParser::GetParsedValue()
{
    return std::move(m_state);
}
