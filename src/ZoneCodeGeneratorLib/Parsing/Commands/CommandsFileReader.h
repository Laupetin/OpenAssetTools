#pragma once

#include "Parsing/IParserLineStream.h"
#include "Parsing/PostProcessing/IPostProcessor.h"
#include "Persistence/IDataRepository.h"
#include "ZoneCodeGeneratorArguments.h"

#include <string>

class CommandsFileReader
{
    static constexpr const char* ZONE_CODE_GENERATOR_DEFINE_NAME = "__zonecodegenerator";
    static constexpr const char* ZONE_CODE_GENERATOR_DEFINE_VALUE = "1";

    const ZoneCodeGeneratorArguments* m_args;
    std::string m_filename;

    std::vector<std::unique_ptr<IParserLineStream>> m_open_streams;
    IParserLineStream* m_stream;

    std::vector<std::unique_ptr<IPostProcessor>> m_post_processors;

    bool OpenBaseStream();
    void SetupStreamProxies();
    void SetupPostProcessors();

public:
    explicit CommandsFileReader(const ZoneCodeGeneratorArguments* args, std::string filename);

    bool ReadCommandsFile(IDataRepository* repository);
};
