#include "CommandsFileReader.h"

#include "Impl/CommandsLexer.h"
#include "Impl/CommandsParser.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Impl/ParserFilesystemStream.h"
#include "Parsing/PostProcessing/CalculateSizeAndAlignPostProcessor.h"
#include "Parsing/PostProcessing/CrossPlatformStructurePostProcessor.h"
#include "Parsing/PostProcessing/LeafsPostProcessor.h"
#include "Parsing/PostProcessing/MarkingRequiredPostProcessor.h"
#include "Parsing/PostProcessing/MemberLeafsPostProcessor.h"
#include "Parsing/PostProcessing/UnionsPostProcessor.h"
#include "Parsing/PostProcessing/UsagesPostProcessor.h"

#include <algorithm>
#include <chrono>
#include <iostream>

namespace
{
    constexpr const char* ZONE_CODE_GENERATOR_DEFINE_NAME = "__zonecodegenerator";
    constexpr const char* ZONE_CODE_GENERATOR_DEFINE_VALUE = "1";
} // namespace

CommandsFileReader::CommandsFileReader(const ZoneCodeGeneratorArguments* args, std::string filename)
    : m_args(args),
      m_filename(std::move(filename)),
      m_stream(nullptr)
{
    SetupPostProcessors();
}

bool CommandsFileReader::OpenBaseStream()
{
    auto stream = std::make_unique<ParserFilesystemStream>(m_filename);
    if (!stream->IsOpen())
    {
        std::cerr << "Could not open commands file\n";
        return false;
    }

    m_stream = stream.get();
    m_open_streams.emplace_back(std::move(stream));
    return true;
}

void CommandsFileReader::SetupStreamProxies()
{
    auto commentProxy = std::make_unique<CommentRemovingStreamProxy>(m_stream);
    auto includeProxy = std::make_unique<IncludingStreamProxy>(commentProxy.get());
    auto definesProxy = std::make_unique<DefinesStreamProxy>(includeProxy.get());
    definesProxy->AddDefine(DefinesStreamProxy::Define(ZONE_CODE_GENERATOR_DEFINE_NAME, ZONE_CODE_GENERATOR_DEFINE_VALUE));

    m_stream = definesProxy.get();

    m_open_streams.emplace_back(std::move(commentProxy));
    m_open_streams.emplace_back(std::move(includeProxy));
    m_open_streams.emplace_back(std::move(definesProxy));
}

void CommandsFileReader::SetupPostProcessors()
{
    // Order is important
    m_post_processors.emplace_back(std::make_unique<CrossPlatformStructurePostProcessor>());
    m_post_processors.emplace_back(std::make_unique<CalculateSizeAndAlignPostProcessor>());
    m_post_processors.emplace_back(std::make_unique<UsagesPostProcessor>());
    m_post_processors.emplace_back(std::make_unique<LeafsPostProcessor>());
    m_post_processors.emplace_back(std::make_unique<MarkingRequiredPostProcessor>());
    m_post_processors.emplace_back(std::make_unique<MemberLeafsPostProcessor>());
    m_post_processors.emplace_back(std::make_unique<UnionsPostProcessor>());
}

bool CommandsFileReader::ReadCommandsFile(IDataRepository* repository)
{
    if (m_args->m_verbose)
    {
        std::cout << std::format("Reading commands file: {}\n", m_filename);
    }

    if (!OpenBaseStream())
        return false;

    SetupStreamProxies();

    const auto lexer = std::make_unique<CommandsLexer>(m_stream);
    const auto parser = std::make_unique<CommandsParser>(lexer.get(), repository);

    const auto start = std::chrono::steady_clock::now();
    const auto result = parser->Parse();
    const auto end = std::chrono::steady_clock::now();

    if (m_args->m_verbose)
        std::cout << std::format("Processing commands took {}ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    if (!result)
        return false;

    return std::ranges::all_of(m_post_processors,
                               [repository](const std::unique_ptr<IPostProcessor>& postProcessor)
                               {
                                   return postProcessor->PostProcess(repository);
                               });
}
