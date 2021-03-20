#pragma once

#include "ZoneDefinitionLexer.h"
#include "ZoneDefinitionParserValue.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Zone/Definition/ZoneDefinition.h"

class ZoneDefinitionParser final : public AbstractParser<ZoneDefinitionParserValue, ZoneDefinition>
{
protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    explicit ZoneDefinitionParser(ZoneDefinitionLexer* lexer);
    std::unique_ptr<ZoneDefinition> GetParsedValue();
};
