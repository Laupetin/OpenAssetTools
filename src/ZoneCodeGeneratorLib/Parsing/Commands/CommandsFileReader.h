#pragma once

#include <string>

#include "ZoneCodeGeneratorArguments.h"
#include "Parsing/IParserLineStream.h"
#include "Persistence/IDataRepository.h"

class CommandsFileReader
{
    static constexpr const char* ZONE_CODE_GENERATOR_DEFINE_NAME = "__zonecodegenerator";
    static constexpr const char* ZONE_CODE_GENERATOR_DEFINE_VALUE = "1";

    const ZoneCodeGeneratorArguments* m_args;
    std::string m_filename;

    std::vector<std::unique_ptr<IParserLineStream>> m_open_streams;
    IParserLineStream* m_stream;

    bool OpenBaseStream();
    void SetupStreamProxies();

public:
    explicit CommandsFileReader(const ZoneCodeGeneratorArguments* args, std::string filename);

    bool ReadCommandsFile(IDataRepository* repository);
};
