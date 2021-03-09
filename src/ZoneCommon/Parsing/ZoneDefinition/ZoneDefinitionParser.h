#pragma once

#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "Parsing/Impl/AbstractParser.h"
#include "Zone/Definition/ZoneDefinition.h"

class ZoneDefinitionParser final : public AbstractParser<SimpleParserValue, ZoneDefinition>
{
protected:
    const std::vector<sequence_t*>& GetTestsForState() override;

public:
    explicit ZoneDefinitionParser(SimpleLexer* lexer);
    std::unique_ptr<ZoneDefinition> GetParsedValue();
};
