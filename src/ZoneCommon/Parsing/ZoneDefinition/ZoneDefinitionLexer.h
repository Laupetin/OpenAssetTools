#pragma once

#include "Parsing/Impl/AbstractLexer.h"
#include "ZoneDefinitionParserValue.h"

class ZoneDefinitionLexer final : public AbstractLexer<ZoneDefinitionParserValue>
{
    std::string ReadField();

protected:
    ZoneDefinitionParserValue GetNextToken() override;

public:
    explicit ZoneDefinitionLexer(IParserLineStream* stream);
};
