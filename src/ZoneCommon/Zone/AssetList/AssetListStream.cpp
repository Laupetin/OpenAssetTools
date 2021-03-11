#include "AssetListStream.h"

AssetListInputStream::AssetListInputStream(std::istream& stream)
    : m_stream(stream)
{
}

bool AssetListInputStream::NextEntry(AssetListEntry& entry) const
{
    std::vector<std::string> row;

    while(true)
    {
        if (!m_stream.NextRow(row))
            return false;

        if (row.empty())
            continue;

        entry.m_type = row[0];
        if (row.size() >= 2)
            entry.m_name = row[1];
        return true;
    }
}

AssetListOutputStream::AssetListOutputStream(std::ostream& stream)
    : m_stream(stream)
{
}

void AssetListOutputStream::WriteEntry(const AssetListEntry& entry)
{
    m_stream.WriteColumn(entry.m_type);
    m_stream.WriteColumn(entry.m_name);
    m_stream.NextRow();
}
