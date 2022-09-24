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
    public:
        static constexpr auto MULTI_TOKEN_AND = 600;
        static constexpr auto MULTI_TOKEN_EQUALS = 601;

    protected:
        const std::vector<sequence_t*>& GetTestsForState() override;

    public:
        StateMapParser(SimpleLexer* lexer, std::string stateMapName, const StateMapLayout& layout);
        _NODISCARD std::unique_ptr<StateMapDefinition> GetStateMapDefinition() const;
        _NODISCARD StateMapParserState* GetState() const;
    };
}
