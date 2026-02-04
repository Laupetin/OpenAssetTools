#include "StateMapReader.h"

#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Matcher/StateMapExpressionMatchers.h"
#include "Parsing/StateMapParser.h"
#include "Utils/Logging/Log.h"

#include <iostream>

using namespace state_map;

StateMapReader::StateMapReader(std::istream& stream, std::string fileName, std::string stateMapName, const StateMapLayout& layout)
    : m_state_map_name(std::move(stateMapName)),
      m_file_name(std::move(fileName)),
      m_state_map_layout(layout)
{
    m_base_stream = std::make_unique<ParserSingleInputStream>(stream, m_file_name);
    m_comment_proxy = std::make_unique<CommentRemovingStreamProxy>(m_base_stream.get());
}

bool StateMapReader::IsValidEndState(const StateMapParserState* state) const
{
    if (state->m_current_rule)
    {
        con::error("In \"{}\": Unclosed rule at end of file!", m_file_name);
        return false;
    }

    if (state->m_in_entry)
    {
        con::error("In \"{}\": Unclosed entry at end of file!", m_file_name);
        return false;
    }

    for (auto i = 0u; i < state->m_layout.m_entry_layout.m_entries.size(); i++)
    {
        if (state->m_definition->m_state_map_entries[i].m_rules.empty())
        {
            con::error("In \"{}\": State map must define a rule for \"{}\"!", m_file_name, state->m_layout.m_entry_layout.m_entries[i].m_name);
            return false;
        }
    }

    return true;
}

std::unique_ptr<StateMapDefinition> StateMapReader::ReadStateMapDefinition() const
{
    SimpleLexer::Config lexerConfig;
    lexerConfig.m_emit_new_line_tokens = false;
    lexerConfig.m_read_strings = false;
    lexerConfig.m_read_integer_numbers = true;
    lexerConfig.m_read_floating_point_numbers = false;
    StateMapExpressionMatchers().ApplyTokensToLexerConfig(lexerConfig);
    const auto lexer = std::make_unique<SimpleLexer>(m_comment_proxy.get(), std::move(lexerConfig));

    const auto parser = std::make_unique<StateMapParser>(lexer.get(), m_state_map_name, m_state_map_layout);

    const auto success = parser->Parse();
    if (!success)
    {
        con::error("Parsing state map file \"{}\" failed!", m_file_name);
        return {};
    }

    const auto* parserEndState = parser->GetState();
    if (!IsValidEndState(parserEndState))
        return nullptr;

    return parser->GetStateMapDefinition();
}
