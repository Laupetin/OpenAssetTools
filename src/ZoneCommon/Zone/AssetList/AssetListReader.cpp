#include "AssetListReader.h"

#include "Csv/CsvStream.h"
#include "Game/IGame.h"
#include "Zone/AssetNameResolver.h"

#include <format>

namespace
{
    class AssetListInputStream
    {
    public:
        AssetListInputStream(std::istream& stream, GameId game)
            : m_stream(stream),
              m_asset_name_resolver(IAssetNameResolver::GetResolverForGame(game))
        {
        }

        bool NextEntry(AssetListEntry& entry, bool* failure) const
        {
            std::vector<std::string> row;
            if (failure)
                *failure = false;

            while (true)
            {
                if (!m_stream.NextRow(row))
                    return false;

                if (row.empty() || (row.size() == 1 && row[0].empty()))
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

    private:
        CsvInputStream m_stream;
        const IAssetNameResolver* m_asset_name_resolver;
    };
} // namespace

AssetListReader::AssetListReader(ISearchPath& searchPath, const GameId game)
    : m_search_path(searchPath),
      m_game(game)
{
}

std::optional<AssetList> AssetListReader::ReadAssetList(const std::string& zoneName, const bool logMissing) const
{
    const auto assetListFileName = std::format("assetlist/{}.csv", zoneName);
    const auto assetListStream = m_search_path.Open(assetListFileName);

    if (assetListStream.IsOpen())
    {
        AssetList assetList;
        const AssetListInputStream stream(*assetListStream.m_stream, m_game);
        AssetListEntry entry;

        bool failure;
        while (stream.NextEntry(entry, &failure))
        {
            assetList.m_entries.emplace_back(std::move(entry));
        }

        if (!failure)
            return assetList;
    }
    else if (logMissing)
        std::cerr << std::format("Failed to open file for assetlist: {}\n", assetListFileName);

    return std::nullopt;
}
