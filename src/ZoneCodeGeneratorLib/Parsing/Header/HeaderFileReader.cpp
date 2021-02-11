#include "HeaderFileReader.h"

#include <iostream>


#include "Parsing/ParsingException.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
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
    if (!stream.IsOpen())
    {
        std::cout << "Could not open header file" << std::endl;
        return false;
    }

    IParserLineStream* lineStream = &stream;

    CommentRemovingStreamProxy commentProxy(lineStream);
    lineStream = &commentProxy;

    IncludingStreamProxy includeProxy(lineStream);
    lineStream = &includeProxy;

    DefinesStreamProxy definesProxy(lineStream);
    definesProxy.AddDefine(ZONE_CODE_GENERATOR_DEFINE_NAME, ZONE_CODE_GENERATOR_DEFINE_VALUE);
    lineStream = &definesProxy;

    try
    {
        while (true)
        {
            auto line = lineStream->NextLine();

            if (line.IsEof())
                break;

            std::cout << "Line " << line.m_filename << ":" << line.m_line_number << ": " << line.m_line << "\n";
        }
    }
    catch (const ParsingException& e)
    {
        std::cout << "Error: " << e.FullMessage() << std::endl;
    }

    return true;
}
