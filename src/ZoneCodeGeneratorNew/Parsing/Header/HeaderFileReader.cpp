#include "HeaderFileReader.h"

#include <iostream>

HeaderFileReader::HeaderFileReader(const ZoneCodeGeneratorArguments* args, std::string filename)
    : m_args(args),
      m_filename(std::move(filename))
{
}

bool HeaderFileReader::ReadHeaderFile(IDataRepository* repository)
{
    std::cout << "Reading header file: " << m_filename << std::endl;
    return true;
}
