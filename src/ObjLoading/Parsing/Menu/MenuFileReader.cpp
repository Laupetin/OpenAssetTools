#include "MenuFileReader.h"

#include "MenuFileParser.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Impl/ParserMultiInputStream.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Simple/SimpleLexer.h"

MenuFileReader::MenuFileReader(std::istream& stream, std::string fileName, include_callback_t includeCallback)
    : m_file_name(std::move(fileName)),
    m_stream(nullptr)
{
    OpenBaseStream(stream, std::move(includeCallback));
    SetupStreamProxies();
    m_stream = m_open_streams.back().get();
}

MenuFileReader::MenuFileReader(std::istream& stream, std::string fileName)
    : m_file_name(std::move(fileName)),
    m_stream(nullptr)
{
    OpenBaseStream(stream, nullptr);
    SetupStreamProxies();
    m_stream = m_open_streams.back().get();
}

bool MenuFileReader::OpenBaseStream(std::istream& stream, include_callback_t includeCallback)
{
    if(includeCallback)
        m_open_streams.emplace_back(std::make_unique<ParserMultiInputStream>(stream, m_file_name, std::move(includeCallback)));
    else
        m_open_streams.emplace_back(std::make_unique<ParserSingleInputStream>(stream, m_file_name));

    return true;
}

void MenuFileReader::SetupStreamProxies()
{
    m_open_streams.emplace_back(std::make_unique<CommentRemovingStreamProxy>(m_open_streams.back().get()));
    m_open_streams.emplace_back(std::make_unique<IncludingStreamProxy>(m_open_streams.back().get()));
    m_open_streams.emplace_back(std::make_unique<DefinesStreamProxy>(m_open_streams.back().get()));

    m_stream = m_open_streams.back().get();
}

bool MenuFileReader::ReadMenuFile(MenuFeatureLevel featureLevel)
{
    const auto lexer = std::make_unique<SimpleLexer>(m_stream, SimpleLexer::Config{false, true, false});
    const auto parser = std::make_unique<MenuFileParser>(lexer.get(), featureLevel);

    if (parser->Parse())
        return true;

    std::cout << "Parsing menu file failed!" << std::endl;
    return false;
}
