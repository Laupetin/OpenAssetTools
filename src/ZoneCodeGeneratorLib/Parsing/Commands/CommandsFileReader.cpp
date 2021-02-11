#include "CommandsFileReader.h"

#include <iostream>

#include "Impl/CommandsLexer.h"
#include "Parsing/ParsingException.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Impl/ParserFilesystemStream.h"

CommandsFileReader::CommandsFileReader(const ZoneCodeGeneratorArguments* args, std::string filename)
    : m_args(args),
      m_filename(std::move(filename)),
      m_stream(nullptr)
{
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

bool CommandsFileReader::ReadCommandsFile(IDataRepository* repository)
{
    std::cout << "Reading commands file: " << m_filename << std::endl;

    if (!OpenBaseStream())
        return false;

    SetupStreamProxies();

    auto lexer = std::make_unique<CommandsLexer>(m_stream);

    try
    {
        while (true)
        {
            auto line = m_stream->NextLine();

            if (line.IsEof())
                break;

            std::cout << "Line " << line.m_filename.get() << ":" << line.m_line_number << ": " << line.m_line << "\n";
        }
    }
    catch (const ParsingException& e)
    {
        std::cout << "Error: " << e.FullMessage() << std::endl;
    }

    return true;
}
