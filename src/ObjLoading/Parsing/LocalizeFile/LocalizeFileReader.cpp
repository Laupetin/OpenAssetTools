#include "LocalizeFileReader.h"

#include "LocalizeFileParser.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/ParserSingleInputStream.h"

LocalizeFileReader::LocalizeFileReader(std::istream& stream, std::string fileName, GameLanguage language)
    : m_file_name(std::move(fileName)),
      m_stream(nullptr),
      m_language(language)
{
    OpenBaseStream(stream);
    SetupStreamProxies();
    m_stream = m_open_streams.back().get();
}

bool LocalizeFileReader::OpenBaseStream(std::istream& stream)
{
    m_open_streams.emplace_back(std::make_unique<ParserSingleInputStream>(stream, m_file_name));
    return true;
}

void LocalizeFileReader::SetupStreamProxies()
{
    m_open_streams.emplace_back(std::make_unique<CommentRemovingStreamProxy>(m_open_streams.back().get()));

    m_stream = m_open_streams.back().get();
}

std::vector<LocalizeFileEntry> LocalizeFileReader::ReadLocalizeFile()
{
    SimpleLexer::Config lexerConfig;
    lexerConfig.m_emit_new_line_tokens = true;
    lexerConfig.m_read_strings = true;
    lexerConfig.m_read_integer_numbers = false;
    lexerConfig.m_read_floating_point_numbers = false;
    const auto lexer = std::make_unique<SimpleLexer>(m_stream, std::move(lexerConfig));

    const auto parser = std::make_unique<LocalizeFileParser>(lexer.get(), m_language);

    if (parser->Parse())
        return parser->GetParsedValues();

    std::cout << "Parsing localization file failed!" << std::endl;
    return std::vector<LocalizeFileEntry>();
}
