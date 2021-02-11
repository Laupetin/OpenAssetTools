#pragma once

#include <string>

#include "ZoneCodeGeneratorArguments.h"
#include "Persistence/IDataRepository.h"

class CommandsFileReader
{
    static constexpr const char* ZONE_CODE_GENERATOR_DEFINE_NAME = "__zonecodegenerator";
    static constexpr const char* ZONE_CODE_GENERATOR_DEFINE_VALUE = "1";

    const ZoneCodeGeneratorArguments* m_args;
    std::string m_filename;

public:
    explicit CommandsFileReader(const ZoneCodeGeneratorArguments* args, std::string filename);

    bool ReadCommandsFile(IDataRepository* repository);
};
