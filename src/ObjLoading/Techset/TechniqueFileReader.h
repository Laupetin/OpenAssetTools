#pragma once

#include "Parsing/IParserLineStream.h"
#include "TechniqueDefinitionAcceptor.h"
#include "Utils/ClassUtils.h"

#include <memory>
#include <string>

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
} // namespace techset
