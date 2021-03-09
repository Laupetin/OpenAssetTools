#include "ZoneDefinitionStream.h"

#include <chrono>

#include "Parsing/Impl/ParserInputStream.h"
#include <Parsing/Impl/CommentRemovingStreamProxy.h>

#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/ZoneDefinition/ZoneDefinitionParser.h"

ZoneDefinitionInputStream::ZoneDefinitionInputStream(std::istream& stream, std::string fileName, bool verbose)
    : m_file_name(std::move(fileName)),
      m_verbose(verbose),
      m_stream(nullptr)
{
    OpenBaseStream(stream);
    SetupStreamProxies();
    m_stream = m_open_streams.back().get();
}

bool ZoneDefinitionInputStream::OpenBaseStream(std::istream& stream)
{
    m_open_streams.emplace_back(std::make_unique<ParserInputStream>(stream, m_file_name));
    return true;
}

void ZoneDefinitionInputStream::SetupStreamProxies()
{
    m_open_streams.emplace_back(std::make_unique<CommentRemovingStreamProxy>(m_open_streams.back().get()));
    auto definesProxy = std::make_unique<DefinesStreamProxy>(m_open_streams.back().get());
    definesProxy->AddDefine(DefinesStreamProxy::Define(ZONE_CODE_GENERATOR_DEFINE_NAME, ZONE_CODE_GENERATOR_DEFINE_VALUE));
    m_open_streams.emplace_back(std::move(definesProxy));

    m_stream = m_open_streams.back().get();
}

std::unique_ptr<ZoneDefinition> ZoneDefinitionInputStream::ReadDefinition()
{
    if (m_verbose)
    {
        std::cout << "Reading zone definition file: " << m_file_name << std::endl;
    }

    const auto lexer = std::make_unique<SimpleLexer>(m_stream);
    const auto parser = std::make_unique<ZoneDefinitionParser>(lexer.get());

    const auto start = std::chrono::steady_clock::now();
    std::unique_ptr<ZoneDefinition> definition;
    if (parser->Parse())
        definition = parser->GetParsedValue();
    const auto end = std::chrono::steady_clock::now();

    if (m_verbose)
    {
        std::cout << "Processing zone definition took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    }

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
    m_stream << entryKey << "," << entryValue << "\n";
}
