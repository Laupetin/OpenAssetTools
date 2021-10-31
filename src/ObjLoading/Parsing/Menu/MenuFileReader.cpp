#include "MenuFileReader.h"

#include "MenuFileParser.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Impl/ParserMultiInputStream.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Simple/SimpleLexer.h"

MenuFileReader::MenuFileReader(std::istream& stream, std::string fileName, MenuFeatureLevel featureLevel, include_callback_t includeCallback)
    : m_feature_level(featureLevel),
      m_file_name(std::move(fileName)),
      m_stream(nullptr)
{
    OpenBaseStream(stream, std::move(includeCallback));
    SetupStreamProxies();
    m_stream = m_open_streams.back().get();
}

MenuFileReader::MenuFileReader(std::istream& stream, std::string fileName, MenuFeatureLevel featureLevel)
    : m_feature_level(featureLevel),
      m_file_name(std::move(fileName)),
      m_stream(nullptr)
{
    OpenBaseStream(stream, nullptr);
    SetupStreamProxies();
    m_stream = m_open_streams.back().get();
}

bool MenuFileReader::OpenBaseStream(std::istream& stream, include_callback_t includeCallback)
{
    if (includeCallback)
        m_open_streams.emplace_back(std::make_unique<ParserMultiInputStream>(stream, m_file_name, std::move(includeCallback)));
    else
        m_open_streams.emplace_back(std::make_unique<ParserSingleInputStream>(stream, m_file_name));

    return true;
}

void MenuFileReader::SetupDefinesProxy()
{
    auto defines = std::make_unique<DefinesStreamProxy>(m_open_streams.back().get());

    defines->AddDefine(DefinesStreamProxy::Define("PC", "1"));
    switch(m_feature_level)
    {
    case MenuFeatureLevel::IW4:
        defines->AddDefine(DefinesStreamProxy::Define("FEATURE_LEVEL_IW4", "1"));
        break;
    case MenuFeatureLevel::IW5:
        defines->AddDefine(DefinesStreamProxy::Define("FEATURE_LEVEL_IW5", "1"));
        break;
    default:
        assert(false);
        break;
    }

    m_open_streams.emplace_back(std::move(defines));
}

void MenuFileReader::SetupStreamProxies()
{
    m_open_streams.emplace_back(std::make_unique<CommentRemovingStreamProxy>(m_open_streams.back().get()));
    m_open_streams.emplace_back(std::make_unique<IncludingStreamProxy>(m_open_streams.back().get()));
    SetupDefinesProxy();

    m_stream = m_open_streams.back().get();
}

bool MenuFileReader::ReadMenuFile()
{
    const auto lexer = std::make_unique<SimpleLexer>(m_stream, SimpleLexer::Config{false, true, false});
    const auto parser = std::make_unique<MenuFileParser>(lexer.get(), m_feature_level);

    if (parser->Parse())
        return true;

    std::cout << "Parsing menu file failed!" << std::endl;
    return false;
}
