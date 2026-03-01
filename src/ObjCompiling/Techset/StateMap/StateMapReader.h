#pragma once

#include "Parsing/IParserLineStream.h"
#include "Parsing/StateMapParserState.h"
#include "StateMapDefinition.h"
#include "Techset/StateMap/StateMapLayout.h"

#include <memory>
#include <string>

namespace state_map
{
    class StateMapReader
    {
        std::string m_state_map_name;
        std::string m_file_name;
        const StateMapLayout& m_state_map_layout;
        std::unique_ptr<IParserLineStream> m_base_stream;
        std::unique_ptr<IParserLineStream> m_comment_proxy;

    public:
        StateMapReader(std::istream& stream, std::string fileName, std::string stateMapName, const StateMapLayout& layout);

        [[nodiscard]] bool IsValidEndState(const StateMapParserState* state) const;
        [[nodiscard]] std::unique_ptr<StateMapDefinition> ReadStateMapDefinition() const;
    };
} // namespace state_map
