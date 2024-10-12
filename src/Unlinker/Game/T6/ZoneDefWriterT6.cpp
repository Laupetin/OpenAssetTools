#include "ZoneDefWriterT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/GameAssetPoolT6.h"

#include <cassert>
#include <iomanip>
#include <sstream>

using namespace T6;

namespace
{
    class KeyValuePairKnownKey
    {
    public:
        std::string m_key;
        int m_hash;

        explicit KeyValuePairKnownKey(std::string key)
        {
            m_key = std::move(key);
            m_hash = Common::Com_HashKey(m_key.c_str(), 64);
        }
    };

    const KeyValuePairKnownKey KEY_VALUE_PAIR_KNOWN_KEYS[]{
        KeyValuePairKnownKey("ipak_read"),
        KeyValuePairKnownKey("ipak_write"),
        KeyValuePairKnownKey("initial_xmodels"),
        KeyValuePairKnownKey("initial_materials"),
    };

    void WriteKeyValuePair(ZoneDefinitionOutputStream& stream, const KeyValuePair& kvp)
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

void ZoneDefWriter::WriteMetaData(ZoneDefinitionOutputStream& stream, const UnlinkerArgs& args, const Zone& zone) const
{
    const auto* assetPoolT6 = dynamic_cast<GameAssetPoolT6*>(zone.m_pools.get());
    if (assetPoolT6 && !assetPoolT6->m_key_value_pairs->m_asset_lookup.empty())
    {
        for (const auto* kvpAsset : *assetPoolT6->m_key_value_pairs)
        {
            const auto* keyValuePairs = kvpAsset->Asset();
            for (auto varIndex = 0; varIndex < keyValuePairs->numVariables; varIndex++)
                WriteKeyValuePair(stream, keyValuePairs->keyValuePairs[varIndex]);
        }

        stream.EmptyLine();
    }
}

void ZoneDefWriter::WriteContent(ZoneDefinitionOutputStream& stream, const UnlinkerArgs& args, const Zone& zone) const
{
    const auto* pools = dynamic_cast<GameAssetPoolT6*>(zone.m_pools.get());

    assert(pools);
    if (!pools)
        return;

    // Localized strings are all collected in one string file. So only add this to the zone file.
    if (!pools->m_localize->m_asset_lookup.empty())
        stream.WriteEntry(*pools->GetAssetTypeName(ASSET_TYPE_LOCALIZE_ENTRY), zone.m_name);

    for (const auto& asset : *pools)
    {
        switch (asset->m_type)
        {
        case ASSET_TYPE_LOCALIZE_ENTRY:
        case ASSET_TYPE_KEYVALUEPAIRS: // KeyValuePairs should be included as zone file metadata and not as content
            break;

        default:
            stream.WriteEntry(*pools->GetAssetTypeName(asset->m_type), asset->m_name);
            break;
        }
    }
}
