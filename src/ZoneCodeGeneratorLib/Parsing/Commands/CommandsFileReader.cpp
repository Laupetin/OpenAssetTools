#include "CommandsFileReader.h"

#include <algorithm>
#include <chrono>
#include <iostream>

#include "Impl/CommandsLexer.h"
#include "Impl/CommandsParser.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Impl/ParserFilesystemStream.h"
#include "Parsing/PostProcessing/CalculateSizeAndAlignPostProcessor.h"
#include "Parsing/PostProcessing/LeafsPostProcessor.h"
#include "Parsing/PostProcessing/MemberLeafsPostProcessor.h"
#include "Parsing/PostProcessing/UnionsPostProcessor.h"
#include "Parsing/PostProcessing/UsagesPostProcessor.h"

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
        std::cout << "Could not open commands file" << std::endl;
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
    definesProxy->AddDefine(ZONE_CODE_GENERATOR_DEFINE_NAME, ZONE_CODE_GENERATOR_DEFINE_VALUE);

    m_stream = definesProxy.get();

    m_open_streams.emplace_back(std::move(commentProxy));
    m_open_streams.emplace_back(std::move(includeProxy));
    m_open_streams.emplace_back(std::move(definesProxy));
}

void CommandsFileReader::SetupPostProcessors()
{
    // Order is important
    m_post_processors.emplace_back(std::make_unique<CalculateSizeAndAlignPostProcessor>());
    m_post_processors.emplace_back(std::make_unique<UsagesPostProcessor>());
    m_post_processors.emplace_back(std::make_unique<LeafsPostProcessor>());
    m_post_processors.emplace_back(std::make_unique<MemberLeafsPostProcessor>());
    m_post_processors.emplace_back(std::make_unique<UnionsPostProcessor>());
}

bool CommandsFileReader::ReadCommandsFile(IDataRepository* repository)
{
    std::cout << "Reading commands file: " << m_filename << std::endl;

    if (!OpenBaseStream())
        return false;

    SetupStreamProxies();

    const auto lexer = std::make_unique<CommandsLexer>(m_stream);
    const auto parser = std::make_unique<CommandsParser>(lexer.get(), repository);

    const auto start = std::chrono::steady_clock::now();
    const auto result = parser->Parse();
    const auto end = std::chrono::steady_clock::now();
    std::cout << "Processing commands took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    if (!result)
        return false;

    return std::all_of(m_post_processors.begin(), m_post_processors.end(), [repository](const std::unique_ptr<IPostProcessor>& postProcessor)
    {
        return postProcessor->PostProcess(repository);
    });
}
