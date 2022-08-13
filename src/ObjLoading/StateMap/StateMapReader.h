#pragma once

#include <memory>
#include <string>

#include "Utils/ClassUtils.h"
#include "StateMapDefinition.h"
#include "StateMap/StateMapLayout.h"
#include "Parsing/IParserLineStream.h"
#include "Parsing/StateMapParserState.h"

namespace state_map
{
    class StateMapReader
    {
        std::string m_file_name;
        const StateMapLayout& m_state_map_layout;
        std::unique_ptr<IParserLineStream> m_base_stream;
        std::unique_ptr<IParserLineStream> m_comment_proxy;

    public:
        StateMapReader(std::istream& stream, std::string fileName, const StateMapLayout& layout);

        _NODISCARD bool IsValidEndState(const StateMapParserState* state) const;
        _NODISCARD std::unique_ptr<StateMapDefinition> ReadStateMapDefinition() const;
    };
}
