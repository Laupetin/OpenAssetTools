#include "StateMapReader.h"

#include <iostream>

#include "Parsing/StateMapParser.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/ParserSingleInputStream.h"

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
        std::cerr << "In \"" << m_file_name << "\": Unclosed rule at end of file!\n";
        return false;
    }

    if (state->m_in_entry)
    {
        std::cerr << "In \"" << m_file_name << "\": Unclosed entry at end of file!\n";
        return false;
    }

    for (auto i = 0u; i < state->m_layout.m_entry_layout.m_entries.size(); i++)
    {
        if (state->m_definition->m_state_map_entries[i].m_rules.empty())
        {
            std::cerr << "In \"" << m_file_name << "\": State map must define a rule for \"" << state->m_layout.m_entry_layout.m_entries[i].m_name << "\"!\n";
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
    const auto lexer = std::make_unique<SimpleLexer>(m_comment_proxy.get(), std::move(lexerConfig));

    const auto parser = std::make_unique<StateMapParser>(lexer.get(), m_state_map_name, m_state_map_layout);

    const auto success = parser->Parse();
    if (!success)
    {
        std::cout << "Parsing state map file \"" << m_file_name << "\" failed!\n";
        return {};
    }

    const auto* parserEndState = parser->GetState();
    if (!IsValidEndState(parserEndState))
        return nullptr;

    return parser->GetStateMapDefinition();
}
