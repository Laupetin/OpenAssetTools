#include "AssetListStream.h"

#include <format>

AssetListInputStream::AssetListInputStream(std::istream& stream, const GameId game)
    : m_stream(stream),
      m_asset_name_resolver(IAssetNameResolver::GetResolverForGame(game))
{
}

bool AssetListInputStream::NextEntry(AssetListEntry& entry, bool* failure) const
{
    std::vector<std::string> row;
    if (failure)
        *failure = false;

    while (true)
    {
        if (!m_stream.NextRow(row))
            return false;

        if (row.empty())
            continue;

        const auto& typeName = row[0];
        const auto maybeType = m_asset_name_resolver->GetAssetTypeByName(typeName);
        if (!maybeType)
        {
            std::cerr << std::format("Unknown asset type name \"{}\"\n", typeName);
            if (failure)
                *failure = true;
            return false;
        }

        entry.m_type = *maybeType;
        if (row.size() >= 3 && row[1].empty())
        {
            entry.m_name = row[2];
            entry.m_is_reference = true;
        }
        else
        {
            entry.m_name = row[1];
            entry.m_is_reference = false;
        }

        return true;
    }
}

AssetListOutputStream::AssetListOutputStream(std::ostream& stream, const GameId game)
    : m_stream(stream),
      m_asset_name_resolver(IAssetNameResolver::GetResolverForGame(game))
{
}

void AssetListOutputStream::WriteEntry(const AssetListEntry& entry)
{
    m_stream.WriteColumn(*m_asset_name_resolver->GetAssetTypeName(entry.m_type));
    m_stream.WriteColumn(entry.m_name);
    m_stream.NextRow();
}
