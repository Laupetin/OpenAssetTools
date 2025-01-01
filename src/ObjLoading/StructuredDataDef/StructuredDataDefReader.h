#pragma once

#include "Parsing/IParserLineStream.h"
#include "SearchPath/ISearchPath.h"
#include "SearchPath/SearchPathMultiInputStream.h"
#include "StructuredDataDef/CommonStructuredDataDef.h"

#include <memory>
#include <string>
#include <vector>

class StructuredDataDefReader : public SearchPathMultiInputStream
{
public:
    StructuredDataDefReader(std::istream& stream, std::string fileName, ISearchPath& searchPath);

    std::vector<std::unique_ptr<CommonStructuredDataDef>> ReadStructureDataDefs(bool& success);

private:
    bool OpenBaseStream(std::istream& stream);
    void SetupStreamProxies();

    std::string m_file_name;
    IParserLineStream* m_stream;
    std::vector<std::unique_ptr<IParserLineStream>> m_open_streams;
};
