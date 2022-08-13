#pragma once

#include "StateMapParserState.h"
#include "Utils/ClassUtils.h"
#include "Techset/TechsetDefinition.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "Parsing/Impl/AbstractParser.h"

namespace state_map
{
    class StateMapParser final : public AbstractParser<SimpleParserValue, StateMapParserState>
    {
    protected:
        const std::vector<sequence_t*>& GetTestsForState() override;

    public:
        StateMapParser(SimpleLexer* lexer, const StateMapLayout& layout);
        _NODISCARD std::unique_ptr<StateMapDefinition> GetStateMapDefinition() const;
        _NODISCARD StateMapParserState* GetState() const;
    };
}
