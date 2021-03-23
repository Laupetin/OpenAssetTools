#include "LocalizeFileReader.h"

#include "LocalizeFileLexer.h"
#include "LocalizeFileParser.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/ParserInputStream.h"

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
    m_open_streams.emplace_back(std::make_unique<ParserInputStream>(stream, m_file_name));
    return true;
}

void LocalizeFileReader::SetupStreamProxies()
{
    m_open_streams.emplace_back(std::make_unique<CommentRemovingStreamProxy>(m_open_streams.back().get()));

    m_stream = m_open_streams.back().get();
}

std::vector<LocalizeFileEntry> LocalizeFileReader::ReadLocalizeFile()
{
    const auto lexer = std::make_unique<LocalizeFileLexer>(m_stream);
    const auto parser = std::make_unique<LocalizeFileParser>(lexer.get(), m_language);

    if (parser->Parse())
        return parser->GetParsedValues();

    std::cout << "Parsing localization file failed!" << std::endl;
    return std::vector<LocalizeFileEntry>();
}
