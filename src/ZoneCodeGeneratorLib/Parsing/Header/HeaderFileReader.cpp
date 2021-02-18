#include "HeaderFileReader.h"


#include <chrono>
#include <iostream>

#include "Impl/HeaderLexer.h"
#include "Impl/HeaderParser.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Impl/PackDefinitionStreamProxy.h"
#include "Parsing/Impl/ParserFilesystemStream.h"

HeaderFileReader::HeaderFileReader(const ZoneCodeGeneratorArguments* args, std::string filename)
    : m_args(args),
      m_filename(std::move(filename)),
      m_pack_value_supplier(nullptr),
      m_stream(nullptr)
{
}

bool HeaderFileReader::OpenBaseStream()
{
    auto stream = std::make_unique<ParserFilesystemStream>(m_filename);
    if (!stream->IsOpen())
    {
        std::cout << "Could not open header file" << std::endl;
        return false;
    }

    m_stream = stream.get();
    m_open_streams.emplace_back(std::move(stream));
    return true;
}

void HeaderFileReader::SetupStreamProxies()
{
    auto commentProxy = std::make_unique<CommentRemovingStreamProxy>(m_stream);
    auto includeProxy = std::make_unique<IncludingStreamProxy>(commentProxy.get());
    auto packProxy = std::make_unique<PackDefinitionStreamProxy>(includeProxy.get());
    auto definesProxy = std::make_unique<DefinesStreamProxy>(packProxy.get());
    definesProxy->AddDefine(ZONE_CODE_GENERATOR_DEFINE_NAME, ZONE_CODE_GENERATOR_DEFINE_VALUE);

    m_pack_value_supplier = packProxy.get();
    m_stream = definesProxy.get();

    m_open_streams.emplace_back(std::move(commentProxy));
    m_open_streams.emplace_back(std::move(includeProxy));
    m_open_streams.emplace_back(std::move(packProxy));
    m_open_streams.emplace_back(std::move(definesProxy));
}

bool HeaderFileReader::ReadHeaderFile(IDataRepository* repository)
{
    std::cout << "Reading header file: " << m_filename << std::endl;

    if (!OpenBaseStream())
        return false;

    SetupStreamProxies();

    const auto lexer = std::make_unique<HeaderLexer>(m_stream);
    const auto parser = std::make_unique<HeaderParser>(lexer.get(), repository);

    const auto start = std::chrono::steady_clock::now();
    const auto result = parser->Parse();
    const auto end = std::chrono::steady_clock::now();
    std::cout << "Processing header took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    return result;
}
