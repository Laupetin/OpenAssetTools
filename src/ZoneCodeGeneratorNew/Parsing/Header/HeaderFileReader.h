#pragma once

#include <string>

#include "ZoneCodeGeneratorArguments.h"
#include "Persistence/IDataRepository.h"

class HeaderFileReader
{
    const ZoneCodeGeneratorArguments* m_args;
    std::string m_filename;

public:
    HeaderFileReader(const ZoneCodeGeneratorArguments* args, std::string filename);

    bool ReadHeaderFile(IDataRepository* repository) const;
};
