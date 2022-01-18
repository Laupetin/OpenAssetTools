#include "StructuredDataDefReader.h"

#include "StructuredDataDefParser.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Impl/ParserMultiInputStream.h"
#include "Parsing/Impl/ParserSingleInputStream.h"

using namespace sdd;

StructuredDataDefReader::StructuredDataDefReader(std::istream& stream, std::string fileName)
    : StructuredDataDefReader(stream, std::move(fileName), nullptr)
{
}

StructuredDataDefReader::StructuredDataDefReader(std::istream& stream, std::string fileName, include_callback_t includeCallback)
    : m_file_name(std::move(fileName)),
      m_stream(nullptr)
{
    OpenBaseStream(stream, std::move(includeCallback));
    SetupStreamProxies();
    m_stream = m_open_streams.back().get();
}

bool StructuredDataDefReader::OpenBaseStream(std::istream& stream, include_callback_t includeCallback)
{
    if (includeCallback)
        m_open_streams.emplace_back(std::make_unique<ParserMultiInputStream>(stream, m_file_name, std::move(includeCallback)));
    else
        m_open_streams.emplace_back(std::make_unique<ParserSingleInputStream>(stream, m_file_name));

    return true;
}

void StructuredDataDefReader::SetupStreamProxies()
{
    m_open_streams.emplace_back(std::make_unique<CommentRemovingStreamProxy>(m_open_streams.back().get()));
    m_open_streams.emplace_back(std::make_unique<IncludingStreamProxy>(m_open_streams.back().get()));
    m_open_streams.emplace_back(std::make_unique<DefinesStreamProxy>(m_open_streams.back().get()));

    m_stream = m_open_streams.back().get();
}

std::vector<std::unique_ptr<CommonStructuredDataDef>> StructuredDataDefReader::ReadStructureDataDefs()
{
    SimpleLexer::Config lexerConfig;
    lexerConfig.m_emit_new_line_tokens = false;
    lexerConfig.m_read_strings = true;
    lexerConfig.m_read_numbers = true;
    const auto lexer = std::make_unique<SimpleLexer>(m_stream, std::move(lexerConfig));

    const auto parser = std::make_unique<StructuredDataDefParser>(lexer.get());

    if (parser->Parse())
        return parser->GetDefs();

    std::cout << "Parsing structured data def file \"" << m_file_name << "\" failed!" << std::endl;
    return {};
}
