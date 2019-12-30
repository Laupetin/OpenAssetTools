#include "ObjLoaderT6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/GameAssetPoolT6.h"

const int ObjLoaderT6::IPAK_READ_HASH = ObjLoaderT6::Com_HashKey("ipak_read", 64);
const int ObjLoaderT6::GLOBAL_HASH = ObjLoaderT6::Com_HashKey("GLOBAL", 64);

int ObjLoaderT6::Com_HashKey(const char* str, const int maxLen)
{
    if (str == nullptr)
        return 0;

    int hash = 0;
    for (int i = 0; i < maxLen; i++)
    {
        if (str[i] == '\0')
            break;

        hash += str[i] * (0x77 + i);
    }

    return hash ^ ((hash ^ (hash >> 10)) >> 10);
}

bool ObjLoaderT6::SupportsZone(Zone* zone)
{
    return zone->m_game == &g_GameT6;
}

void ObjLoaderT6::LoadIPakForZone(const std::string& ipakName, Zone* zone)
{
    printf("Loading ipak '%s' for zone '%s'\n", ipakName.c_str(), zone->m_name.c_str());
    // TODO
}

void ObjLoaderT6::LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone)
{
    auto* assetPoolT6 = dynamic_cast<GameAssetPoolT6*>(zone->GetPools());
    const int zoneNameHash = Com_HashKey(zone->m_name.c_str(), 64);

    if(assetPoolT6->m_key_value_pairs != nullptr)
    {
        for(auto* keyValuePairs : *assetPoolT6->m_key_value_pairs)
        {
            for(int variableIndex = 0; variableIndex < keyValuePairs->m_asset->numVariables; variableIndex++)
            {
                T6::KeyValuePair* variable = &keyValuePairs->m_asset->keyValuePairs[variableIndex];
                
                if(variable->namespaceHash == zoneNameHash && variable->keyHash == IPAK_READ_HASH)
                {
                    LoadIPakForZone(variable->value, zone);
                }
            }
        }
    }
}

void ObjLoaderT6::LoadObjDataForZone(ISearchPath* searchPath, Zone* zone)
{
    // TODO
}
