#include "ZoneDefWriterT6.h"

#include "Game/T6/CommonT6.h"

#include <iomanip>
#include <sstream>

using namespace T6;

namespace
{
    class KeyValuePairKnownKey
    {
    public:
        std::string m_key;
        unsigned m_hash;

        explicit KeyValuePairKnownKey(std::string key)
        {
            m_key = std::move(key);
            m_hash = static_cast<unsigned>(Common::Com_HashKey(m_key.c_str(), 64));
        }
    };

    const KeyValuePairKnownKey KEY_VALUE_PAIR_KNOWN_KEYS[]{
        KeyValuePairKnownKey("ipak_read"),
        KeyValuePairKnownKey("ipak_write"),
        KeyValuePairKnownKey("initial_xmodels"),
        KeyValuePairKnownKey("initial_materials"),
    };

    void WriteKeyValuePair(const ZoneDefinitionOutputStream& stream, const KeyValuePair& kvp)
    {
        for (const auto& knownKey : KEY_VALUE_PAIR_KNOWN_KEYS)
        {
            if (knownKey.m_hash == kvp.keyHash)
            {
                stream.WriteMetaData("level." + knownKey.m_key, kvp.value);
                return;
            }
        }

        std::ostringstream str;
        str << "level.@" << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << kvp.keyHash;
        stream.WriteMetaData(str.str(), kvp.value);
    }
} // namespace

void ZoneDefWriter::WriteMetaData(ZoneDefinitionOutputStream& stream, const Zone& zone) const
{
    auto kvpPoolAssets = zone.m_pools.PoolAssets<AssetKeyValuePairs>();
    if (kvpPoolAssets.begin() != kvpPoolAssets.end())
    {
        for (const auto* kvpAsset : kvpPoolAssets)
        {
            const auto* keyValuePairs = kvpAsset->Asset();
            for (auto varIndex = 0u; varIndex < keyValuePairs->numVariables; varIndex++)
                WriteKeyValuePair(stream, keyValuePairs->keyValuePairs[varIndex]);
        }

        stream.EmptyLine();
    }
}

void ZoneDefWriter::WriteContent(ZoneDefinitionOutputStream& stream, const Zone& zone) const
{
    const auto* game = IGame::GetGameById(zone.m_game_id);

    // Localized strings are all collected in one string file. So only add this to the zone file.
    auto localizePoolAssets = zone.m_pools.PoolAssets<AssetLocalize>();
    if (localizePoolAssets.begin() != localizePoolAssets.end())
        stream.WriteEntry(*game->GetAssetTypeName(ASSET_TYPE_LOCALIZE_ENTRY), zone.m_name);

    for (const auto& asset : zone.m_pools)
    {
        switch (asset->m_type)
        {
        case ASSET_TYPE_LOCALIZE_ENTRY:
        case ASSET_TYPE_KEYVALUEPAIRS: // KeyValuePairs should be included as zone file metadata and not as content
            break;

        default:
            stream.WriteEntry(*game->GetAssetTypeName(asset->m_type), asset->m_name);
            break;
        }
    }
}
