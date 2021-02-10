#pragma once

#include <string>

#include "ZoneCodeGeneratorArguments.h"
#include "Persistence/IDataRepository.h"

class HeaderFileReader
{
    static constexpr const char* ZONE_CODE_GENERATOR_DEFINE_NAME = "__zonecodegenerator";
    static constexpr const char* ZONE_CODE_GENERATOR_DEFINE_VALUE = "1";

    const ZoneCodeGeneratorArguments* m_args;
    std::string m_filename;

public:
    HeaderFileReader(const ZoneCodeGeneratorArguments* args, std::string filename);

    bool ReadHeaderFile(IDataRepository* repository) const;
};
