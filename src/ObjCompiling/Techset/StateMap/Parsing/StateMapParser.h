#pragma once

#include "Parsing/Impl/AbstractParser.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "StateMapParserState.h"

namespace state_map
{
    class StateMapParser final : public AbstractParser<SimpleParserValue, StateMapParserState>
    {
    protected:
        const std::vector<sequence_t*>& GetTestsForState() override;

    public:
        StateMapParser(SimpleLexer* lexer, std::string stateMapName, const StateMapLayout& layout);
        [[nodiscard]] std::unique_ptr<StateMapDefinition> GetStateMapDefinition() const;
        [[nodiscard]] StateMapParserState* GetState() const;
    };
} // namespace state_map
