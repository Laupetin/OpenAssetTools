#include "CommandsFileReader.h"

#include <iostream>

CommandsFileReader::CommandsFileReader(const ZoneCodeGeneratorArguments* args, std::string filename)
    : m_args(args),
      m_filename(std::move(filename))
{
}

bool CommandsFileReader::ReadCommandsFile(IDataRepository* repository)
{
    std::cout << "Reading commands file: " << m_filename << std::endl;
    return true;
}
