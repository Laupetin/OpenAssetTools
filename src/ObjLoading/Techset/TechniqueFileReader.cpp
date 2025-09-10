#include "TechniqueFileReader.h"

#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/TechniqueFileParser.h"
#include "Utils/Logging/Log.h"

#include <iostream>

using namespace techset;

TechniqueFileReader::TechniqueFileReader(std::istream& stream, std::string fileName, ITechniqueDefinitionAcceptor* acceptor)
    : m_file_name(std::move(fileName)),
      m_acceptor(acceptor)
{
    m_base_stream = std::make_unique<ParserSingleInputStream>(stream, m_file_name);
    m_comment_proxy = std::make_unique<CommentRemovingStreamProxy>(m_base_stream.get());
}

bool TechniqueFileReader::ReadTechniqueDefinition() const
{
    SimpleLexer::Config lexerConfig;
    lexerConfig.m_emit_new_line_tokens = false;
    lexerConfig.m_read_strings = true;
    lexerConfig.m_string_escape_sequences = false;
    lexerConfig.m_read_integer_numbers = true;
    lexerConfig.m_read_floating_point_numbers = true;
    const auto lexer = std::make_unique<SimpleLexer>(m_comment_proxy.get(), std::move(lexerConfig));

    const auto parser = std::make_unique<TechniqueParser>(lexer.get(), m_acceptor);

    const auto success = parser->Parse();
    if (success)
        return true;

    con::error("Parsing technique file \"{}\" failed!", m_file_name);
    return false;
}
