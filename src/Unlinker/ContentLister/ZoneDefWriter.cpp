#include "ZoneDefWriter.h"

const std::string AbstractZoneDefWriter::META_DATA_KEY_GAME = "game";

AbstractZoneDefWriter::AbstractZoneDefWriter(Zone* zone, std::ostream& stream)
    : m_zone(zone),
      m_stream(stream)
{
}

void AbstractZoneDefWriter::EmptyLine() const
{
    m_stream << "\n";
}

void AbstractZoneDefWriter::WriteComment(const std::string& comment) const
{
    m_stream << "// " << comment << "\n";
}

void AbstractZoneDefWriter::WriteMetaData(const std::string& metaDataKey, const std::string& metaDataValue) const
{
    m_stream << ">" << metaDataKey << "," << metaDataValue << "\n";
}

void AbstractZoneDefWriter::WriteEntry(const std::string& entryKey, const std::string& entryValue) const
{
    m_stream << entryKey << "," << entryValue << "\n";
}
