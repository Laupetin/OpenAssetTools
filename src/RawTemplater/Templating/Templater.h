#pragma once
#include "Parsing/IParserLineStream.h"
#include "Utils/ClassUtils.h"

#include <string>

namespace templating
{
    class Templater
    {
    public:
        Templater(std::istream& stream, std::string fileName);

        void SetBuildLogFile(std::ostream* buildLogFile);
        _NODISCARD bool TemplateToDirectory(const std::string& outputDirectory) const;

    private:
        std::istream& m_stream;
        std::ostream* m_build_log;
        std::string m_file_name;
    };
} // namespace templating
