#include "HeaderFileReader.h"

#include <iostream>


#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Impl/ParserFilesystemStream.h"

HeaderFileReader::HeaderFileReader(const ZoneCodeGeneratorArguments* args, std::string filename)
    : m_args(args),
      m_filename(std::move(filename))
{
}

bool HeaderFileReader::ReadHeaderFile(IDataRepository* repository) const
{
    std::cout << "Reading header file: " << m_filename << std::endl;

    ParserFilesystemStream stream(m_filename);
    if(!stream.IsOpen())
    {
        std::cout << "Could not open header file" << std::endl;
        return false;
    }

    IncludingStreamProxy includeProxy(&stream);
    IParserLineStream* lineStream = &includeProxy;

    while(true)
    {
        auto line = lineStream->NextLine();
        
        if (line.IsEof())
            break;

        std::cout << "Line " << line.m_filename << ":" << line.m_line_number << ": " << line.m_line << std::endl;
    }

    return true;
}
