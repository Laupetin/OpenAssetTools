#pragma once

#include <memory>
#include <string>

#include "Utils/ClassUtils.h"
#include "TechsetDefinition.h"
#include "Parsing/IParserLineStream.h"

namespace techset
{
    class TechsetFileReader
    {
        std::string m_file_name;
        const char** m_valid_technique_type_names;
        size_t m_valid_technique_type_name_count;
        std::unique_ptr<IParserLineStream> m_base_stream;
        std::unique_ptr<IParserLineStream> m_comment_proxy;

    public:
        TechsetFileReader(std::istream& stream, std::string fileName, const char** validTechniqueTypeNames, size_t validTechniqueTypeNameCount);

        _NODISCARD std::unique_ptr<TechsetDefinition> ReadTechsetDefinition() const;
    };
}
