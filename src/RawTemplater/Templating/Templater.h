#pragma once
#include <string>

#include "Parsing/IParserLineStream.h"

namespace templating
{
    class Templater
    {
    public:
        Templater(std::istream& stream, std::string fileName);
        
        bool TemplateToDirectory(const std::string& outputDirectory);

    private:
        std::istream& m_stream;
        std::string m_file_name;
    };
}
