#pragma once
#include <iostream>
#include <memory>

#include "ZoneDefinition.h"

class ZoneDefinitionInputStream
{
    std::istream& m_stream;

public:
    explicit ZoneDefinitionInputStream(std::istream& stream);

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