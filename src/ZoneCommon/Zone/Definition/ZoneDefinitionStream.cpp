#include "ZoneDefinitionStream.h"

#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/ZoneDefinition/ZoneDefinitionLexer.h"
#include "Parsing/ZoneDefinition/ZoneDefinitionParser.h"

#include <chrono>
#include <format>

ZoneDefinitionInputStream::ZoneDefinitionInputStream(std::istream& stream, std::string targetName, std::string fileName, const bool verbose)
    : m_target_name(std::move(targetName)),
      m_file_name(std::move(fileName)),
      m_verbose(verbose),
      m_stream(nullptr),
      m_previously_set_game(std::nullopt)
{
    OpenBaseStream(stream);
    SetupStreamProxies();
    m_stream = m_open_streams.back().get();
}

bool ZoneDefinitionInputStream::OpenBaseStream(std::istream& stream)
{
    m_open_streams.emplace_back(std::make_unique<ParserSingleInputStream>(stream, m_file_name));
    return true;
}

void ZoneDefinitionInputStream::SetupStreamProxies()
{
    m_open_streams.emplace_back(std::make_unique<CommentRemovingStreamProxy>(m_open_streams.back().get()));
    auto definesProxy = std::make_unique<DefinesStreamProxy>(m_open_streams.back().get());
    m_open_streams.emplace_back(std::move(definesProxy));

    m_stream = m_open_streams.back().get();
}

void ZoneDefinitionInputStream::SetPreviouslySetGame(GameId game)
{
    m_previously_set_game = game;
}

std::unique_ptr<ZoneDefinition> ZoneDefinitionInputStream::ReadDefinition()
{
    if (m_verbose)
        std::cout << std::format("Reading zone definition file: {}\n", m_file_name);

    const auto lexer = std::make_unique<ZoneDefinitionLexer>(m_stream);
    const auto parser = std::make_unique<ZoneDefinitionParser>(lexer.get(), m_target_name, m_previously_set_game);

    const auto start = std::chrono::steady_clock::now();
    std::unique_ptr<ZoneDefinition> definition;
    if (parser->Parse())
        definition = parser->GetParsedValue();
    const auto end = std::chrono::steady_clock::now();

    if (m_verbose)
        std::cout << std::format("Processing zone definition took {}ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    return std::move(definition);
}

ZoneDefinitionOutputStream::ZoneDefinitionOutputStream(std::ostream& stream)
    : m_stream(stream)
{
}

void ZoneDefinitionOutputStream::EmptyLine() const
{
    m_stream << "\n";
}

void ZoneDefinitionOutputStream::WriteComment(const std::string& comment) const
{
    m_stream << "// " << comment << "\n";
}

void ZoneDefinitionOutputStream::WriteMetaData(const std::string& metaDataKey, const std::string& metaDataValue) const
{
    m_stream << ">" << metaDataKey << "," << metaDataValue << "\n";
}

void ZoneDefinitionOutputStream::WriteEntry(const std::string& entryKey, const std::string& entryValue) const
{
    m_stream << entryKey << ",";

    if (entryValue.find('"') != std::string::npos || entryValue.find("//") != std::string::npos)
    {
        m_stream << '"';
        for (const auto& c : entryValue)
        {
            switch (c)
            {
            case '"':
                m_stream << "\\\"";
                break;

            case '\\':
                m_stream << "\\\\";
                break;

            default:
                m_stream << c;
                break;
            }
        }
        m_stream << '"';
    }
    else if (entryValue.empty())
    {
        m_stream << R"("")";
    }
    else
    {
        m_stream << entryValue;
    }

    m_stream << "\n";
}
