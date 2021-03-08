#include "ZoneDefinitionStream.h"

ZoneDefinitionInputStream::ZoneDefinitionInputStream(std::istream& stream)
    : m_stream(stream)
{
}

std::unique_ptr<ZoneDefinition> ZoneDefinitionInputStream::ReadDefinition()
{
    return nullptr;
}

ZoneDefinitionOutputStream::ZoneDefinitionOutputStream(std::ostream& stream)
    : m_stream(stream)
{
}

void ZoneDefinitionOutputStream::EmptyLine() const
{
    m_stream << "\n";
}

void ZoneDefinitionOutputStream::WriteComment(const std::string& comment) const
{
    m_stream << "// " << comment << "\n";
}

void ZoneDefinitionOutputStream::WriteMetaData(const std::string& metaDataKey, const std::string& metaDataValue) const
{
    m_stream << ">" << metaDataKey << "," << metaDataValue << "\n";
}

void ZoneDefinitionOutputStream::WriteEntry(const std::string& entryKey, const std::string& entryValue) const
{
    m_stream << entryKey << "," << entryValue << "\n";
}
