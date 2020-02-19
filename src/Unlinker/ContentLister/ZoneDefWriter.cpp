#include "ZoneDefWriter.h"

const std::string AbstractZoneDefWriter::META_DATA_KEY_GAME = "game";

AbstractZoneDefWriter::AbstractZoneDefWriter(Zone* zone, FileAPI::IFile* file)
{
    m_zone = zone;
    m_file = file;
}

void AbstractZoneDefWriter::EmptyLine() const
{
    m_file->Printf("\n");
}

void AbstractZoneDefWriter::WriteComment(const std::string& comment) const
{
    m_file->Printf("// %s\n", comment.c_str());
}

void AbstractZoneDefWriter::WriteMetaData(const std::string& metaDataKey, const std::string& metaDataValue) const
{
    m_file->Printf(">%s,%s\n", metaDataKey.c_str(), metaDataValue.c_str());
}

void AbstractZoneDefWriter::WriteEntry(const std::string& entryKey, const std::string& entryValue) const
{
    m_file->Printf("%s,%s\n", entryKey.c_str(), entryValue.c_str());
}

void AbstractZoneDefWriter::WriteContent() const
{
    const auto* pools = m_zone->GetPools();

    for(const auto& asset : *pools)
    {
        WriteEntry(pools->GetAssetTypeName(asset->m_type), asset->m_name);
    }
}
