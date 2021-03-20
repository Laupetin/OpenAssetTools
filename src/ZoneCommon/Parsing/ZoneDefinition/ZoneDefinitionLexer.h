#pragma once

#include "ZoneDefinitionParserValue.h"
#include "Parsing/Impl/AbstractLexer.h"

class ZoneDefinitionLexer final : public AbstractLexer<ZoneDefinitionParserValue>
{
    std::string ReadField();

protected:
    ZoneDefinitionParserValue GetNextToken() override;

public:
    explicit ZoneDefinitionLexer(IParserLineStream* stream);
};
