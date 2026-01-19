#pragma once

#include "Parsing/Impl/AbstractParser.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "TechniqueFileParserState.h"

namespace techset
{
    class TechniqueParser final : public AbstractParser<SimpleParserValue, TechniqueParserState>
    {
    protected:
        const std::vector<sequence_t*>& GetTestsForState() override;

    public:
        TechniqueParser(SimpleLexer* lexer, ITechniqueDefinitionAcceptor* acceptor);
    };
} // namespace techset
