#pragma once

#include <memory>
#include <string>

#include "Utils/ClassUtils.h"
#include "TechniqueDefinitionAcceptor.h"
#include "Parsing/IParserLineStream.h"

namespace techset
{
    class TechniqueFileReader
    {
        std::string m_file_name;
        ITechniqueDefinitionAcceptor* m_acceptor;
        std::unique_ptr<IParserLineStream> m_base_stream;
        std::unique_ptr<IParserLineStream> m_comment_proxy;

    public:
        TechniqueFileReader(std::istream& stream, std::string fileName, ITechniqueDefinitionAcceptor* acceptor);

        _NODISCARD bool ReadTechniqueDefinition() const;
    };
}