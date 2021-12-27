#include "MenuFileReader.h"

#include "MenuFileParser.h"
#include "Matcher/MenuExpressionMatchers.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Impl/ParserMultiInputStream.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Simple/SimpleLexer.h"

using namespace menu;

MenuFileReader::MenuFileReader(std::istream& stream, std::string fileName, const FeatureLevel featureLevel, include_callback_t includeCallback)
    : m_feature_level(featureLevel),
      m_file_name(std::move(fileName)),
      m_stream(nullptr),
      m_zone_state(nullptr),
      m_permissive_mode(false)
{
    OpenBaseStream(stream, std::move(includeCallback));
    SetupStreamProxies();
    m_stream = m_open_streams.back().get();
}

MenuFileReader::MenuFileReader(std::istream& stream, std::string fileName, const FeatureLevel featureLevel)
    : m_feature_level(featureLevel),
      m_file_name(std::move(fileName)),
      m_stream(nullptr),
      m_zone_state(nullptr),
      m_permissive_mode(false)
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
    switch (m_feature_level)
    {
    case FeatureLevel::IW4:
        defines->AddDefine(DefinesStreamProxy::Define("FEATURE_LEVEL_IW4", "1"));
        break;
    case FeatureLevel::IW5:
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

bool MenuFileReader::IsValidEndState(const MenuFileParserState* state) const
{
    if (state->m_current_item)
    {
        std::cerr << "In \"" << m_file_name << "\": Unclosed item at end of file!\n";
        return false;
    }

    if (state->m_current_menu)
    {
        std::cerr << "In \"" << m_file_name << "\": Unclosed menu at end of file!\n";
        return false;
    }

    if (state->m_current_function)
    {
        std::cerr << "In \"" << m_file_name << "\": Unclosed function at end of file!\n";
        return false;
    }

    if (state->m_in_global_scope)
    {
        std::cerr << "In \"" << m_file_name << "\": Did not close global scope!\n";
        return false;
    }

    return true;
}

std::unique_ptr<ParsingResult> MenuFileReader::CreateParsingResult(MenuFileParserState* state) const
{
    auto result = std::make_unique<ParsingResult>();
    result->m_menus = std::move(state->m_menus);
    result->m_functions = std::move(state->m_functions);
    result->m_menus_to_load = std::move(state->m_menus_to_load);

    return result;
}

void MenuFileReader::IncludeZoneState(const MenuAssetZoneState* zoneState)
{
    m_zone_state = zoneState;
}

void MenuFileReader::SetPermissiveMode(const bool usePermissiveMode)
{
    m_permissive_mode = usePermissiveMode;
}

std::unique_ptr<ParsingResult> MenuFileReader::ReadMenuFile()
{
    SimpleLexer::Config lexerConfig;
    lexerConfig.m_emit_new_line_tokens = false;
    lexerConfig.m_read_strings = true;
    lexerConfig.m_read_numbers = true;
    MenuExpressionMatchers().ApplyTokensToLexerConfig(lexerConfig);

    const auto lexer = std::make_unique<SimpleLexer>(m_stream, std::move(lexerConfig));
    const auto parser = std::make_unique<MenuFileParser>(lexer.get(), m_feature_level, m_permissive_mode, m_zone_state);

    if (!parser->Parse())
    {
        std::cerr << "Parsing menu file failed!" << std::endl;

        const auto* parserEndState = parser->GetState();
        if(parserEndState->m_current_event_handler_set && !parserEndState->m_permissive_mode)
            std::cerr << "You can use the --menu-permissive option to try to compile the event handler script anyway." << std::endl;
        return nullptr;
    }

    auto* parserEndState = parser->GetState();
    if (!IsValidEndState(parserEndState))
        return nullptr;

    return CreateParsingResult(parserEndState);
}
