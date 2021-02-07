#pragma once

#include <string>

#include "ZoneCodeGeneratorArguments.h"
#include "Persistence/IDataRepository.h"

class CommandsFileReader
{
    const ZoneCodeGeneratorArguments* m_args;
    std::string m_filename;

public:
    explicit CommandsFileReader(const ZoneCodeGeneratorArguments* args, std::string filename);

    bool ReadCommandsFile(IDataRepository* repository);
};
