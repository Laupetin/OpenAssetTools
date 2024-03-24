#include "HeaderFileReader.h"

#include "Impl/HeaderLexer.h"
#include "Impl/HeaderParser.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Impl/PackDefinitionStreamProxy.h"
#include "Parsing/Impl/ParserFilesystemStream.h"
#include "Parsing/PostProcessing/CreateMemberInformationPostProcessor.h"
#include "Parsing/PostProcessing/CreateStructureInformationPostProcessor.h"
#include "Parsing/PostProcessing/IPostProcessor.h"

#include <algorithm>
#include <chrono>
#include <iostream>

HeaderFileReader::HeaderFileReader(const ZoneCodeGeneratorArguments* args, std::string filename)
    : m_args(args),
      m_filename(std::move(filename)),
      m_pack_value_supplier(nullptr),
      m_stream(nullptr)
{
    SetupPostProcessors();
}

bool HeaderFileReader::OpenBaseStream()
{
    auto stream = std::make_unique<ParserFilesystemStream>(m_filename);
    if (!stream->IsOpen())
    {
        std::cout << "Could not open header file\n";
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
    definesProxy->AddDefine(DefinesStreamProxy::Define(ZONE_CODE_GENERATOR_DEFINE_NAME, ZONE_CODE_GENERATOR_DEFINE_VALUE));

    m_pack_value_supplier = packProxy.get();
    m_stream = definesProxy.get();

    m_open_streams.emplace_back(std::move(commentProxy));
    m_open_streams.emplace_back(std::move(includeProxy));
    m_open_streams.emplace_back(std::move(packProxy));
    m_open_streams.emplace_back(std::move(definesProxy));
}

void HeaderFileReader::SetupPostProcessors()
{
    // Order is important
    m_post_processors.emplace_back(std::make_unique<CreateStructureInformationPostProcessor>());
    m_post_processors.emplace_back(std::make_unique<CreateMemberInformationPostProcessor>());
}

bool HeaderFileReader::ReadHeaderFile(IDataRepository* repository)
{
    if (m_args->m_verbose)
    {
        std::cout << "Reading header file: " << m_filename << "\n";
    }

    if (!OpenBaseStream())
        return false;

    SetupStreamProxies();

    const auto lexer = std::make_unique<HeaderLexer>(m_stream);
    const auto parser = std::make_unique<HeaderParser>(lexer.get(), m_pack_value_supplier);

    const auto start = std::chrono::steady_clock::now();
    auto result = parser->Parse();
    if (result)
        result = parser->SaveToRepository(repository);
    const auto end = std::chrono::steady_clock::now();

    if (m_args->m_verbose)
    {
        std::cout << "Processing header took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    }

    if (!result)
        return false;

    return std::all_of(m_post_processors.begin(),
                       m_post_processors.end(),
                       [repository](const std::unique_ptr<IPostProcessor>& postProcessor)
                       {
                           return postProcessor->PostProcess(repository);
                       });
}
