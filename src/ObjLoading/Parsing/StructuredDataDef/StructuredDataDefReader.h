#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Domain/CommonStructuredDataDef.h"
#include "Parsing/IParserLineStream.h"

class StructuredDataDefReader
{
public:
    using include_callback_t = std::function<std::unique_ptr<std::istream>(const std::string& filename, const std::string& sourceFile)>;

private:
    std::string m_file_name;
    IParserLineStream* m_stream;
    std::vector<std::unique_ptr<IParserLineStream>> m_open_streams;

    bool OpenBaseStream(std::istream& stream, include_callback_t includeCallback);
    void SetupStreamProxies();

public:
    StructuredDataDefReader(std::istream& stream, std::string fileName);
    StructuredDataDefReader(std::istream& stream, std::string fileName, include_callback_t includeCallback);

    std::vector<std::unique_ptr<CommonStructuredDataDef>> ReadStructureDataDefs(bool& success);
};
