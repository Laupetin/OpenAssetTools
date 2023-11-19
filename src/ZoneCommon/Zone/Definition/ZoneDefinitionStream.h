#pragma once
#include "Parsing/IParserLineStream.h"
#include "ZoneDefinition.h"

#include <iostream>
#include <memory>

class ZoneDefinitionInputStream
{
    static constexpr const char* ZONE_CODE_GENERATOR_DEFINE_NAME = "__zonecodegenerator";
    static constexpr const char* ZONE_CODE_GENERATOR_DEFINE_VALUE = "1";

    std::string m_file_name;
    bool m_verbose;
    IParserLineStream* m_stream;
    std::vector<std::unique_ptr<IParserLineStream>> m_open_streams;

    bool OpenBaseStream(std::istream& stream);
    void SetupStreamProxies();

public:
    ZoneDefinitionInputStream(std::istream& stream, std::string fileName, bool verbose);

    std::unique_ptr<ZoneDefinition> ReadDefinition();
};

class ZoneDefinitionOutputStream
{
    std::ostream& m_stream;

public:
    explicit ZoneDefinitionOutputStream(std::ostream& stream);

    void EmptyLine() const;
    void WriteComment(const std::string& comment) const;
    void WriteMetaData(const std::string& metaDataKey, const std::string& metaDataValue) const;
    void WriteEntry(const std::string& entryKey, const std::string& entryValue) const;
};
