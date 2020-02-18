#include "ZoneDefWriterT6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/GameAssetPoolT6.h"

#include <sstream>
#include <iomanip>

using namespace T6;

class ZoneDefWriterT6Impl final : public AbstractZoneDefWriter
{
    class KnownKey
    {
    public:
        std::string m_key;
        int m_hash;

        explicit KnownKey(std::string key)
        {
            m_key = std::move(key);
            m_hash = CommonT6::Com_HashKey(m_key.c_str(), 64);
        }
    };

    inline static const KnownKey KNOWN_KEYS[]
    {
        KnownKey("ipak_read"),
        KnownKey("ipak_write"),
        KnownKey("initial_xmodels"),
        KnownKey("initial_materials"),
    };

    void WriteKeyValuePair(KeyValuePair* kvp) const
    {
        for (const auto& knownKey : KNOWN_KEYS)
        {
            if(knownKey.m_hash == kvp->keyHash)
            {
                WriteMetaData("level." + knownKey.m_key, kvp->value);
                return;
            }
        }

        std::ostringstream str;
        str << "level.@" << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << kvp->keyHash;
        WriteMetaData(str.str(), kvp->value);
    }

public:
    ZoneDefWriterT6Impl(Zone* zone, FileAPI::IFile* file)
        : AbstractZoneDefWriter(zone, file)
    {
    }

    void WriteZoneDef() override
    {
        WriteComment("Call Of Duty: Black Ops II");
        WriteMetaData(META_DATA_KEY_GAME, "t6");
        EmptyLine();

        auto* assetPoolT6 = dynamic_cast<GameAssetPoolT6*>(m_zone->GetPools());

        if(assetPoolT6 && !assetPoolT6->m_key_value_pairs->m_asset_lookup.empty())
        {
            for (auto kvpAsset : *assetPoolT6->m_key_value_pairs)
            {
                KeyValuePairs* keyValuePairs = kvpAsset->Asset();
                for(int varIndex = 0; varIndex < keyValuePairs->numVariables; varIndex++)
                {
                    WriteKeyValuePair(&keyValuePairs->keyValuePairs[varIndex]);
                }
            }

            EmptyLine();
        }

        WriteContent();
    }
};

bool ZoneDefWriterT6::CanHandleZone(Zone* zone) const
{
    return zone->m_game == &g_GameT6;
}

void ZoneDefWriterT6::WriteZoneDef(Zone* zone, FileAPI::IFile* file) const
{
    ZoneDefWriterT6Impl writer(zone, file);
    writer.WriteZoneDef();
}
