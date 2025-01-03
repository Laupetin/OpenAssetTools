#include "KeyValuePairsCompilerT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/T6.h"

#include <cassert>
#include <format>
#include <iostream>

using namespace T6;

KeyValuePairsCompiler::KeyValuePairsCompiler(MemoryManager& memory,
                                             const Zone& zone,
                                             const ZoneDefinition& zoneDefinition,
                                             ZoneAssetCreationStateContainer& zoneStates)
    : m_memory(memory),
      m_zone(zone),
      m_zone_definition(zoneDefinition),
      m_kvp_creator(zoneStates.GetZoneAssetCreationState<KeyValuePairsCreator>())
{
}

std::optional<asset_type_t> KeyValuePairsCompiler::GetHandlingAssetType() const
{
    return std::nullopt;
}

AssetCreationResult KeyValuePairsCompiler::CreateAsset(const std::string& assetName, AssetCreationContext& context)
{
    return AssetCreationResult::NoAction();
}

void KeyValuePairsCompiler::FinalizeZone(AssetCreationContext& context)
{
    m_kvp_creator.Finalize(m_zone_definition);
    const auto commonKvps = m_kvp_creator.GetFinalKeyValuePairs();
    if (commonKvps.empty())
        return;

    auto* gameKvps = m_memory.Alloc<KeyValuePairs>();
    gameKvps->name = m_memory.Dup(m_zone.m_name.c_str());
    gameKvps->numVariables = commonKvps.size();
    gameKvps->keyValuePairs = m_memory.Alloc<KeyValuePair>(commonKvps.size());

    const auto namespaceHash = Common::Com_HashKey(m_zone.m_name.c_str(), 64);
    for (auto kvpIndex = 0u; kvpIndex < gameKvps->numVariables; kvpIndex++)
    {
        const auto& commonKvp = commonKvps[kvpIndex];
        auto& gameKvp = gameKvps->keyValuePairs[kvpIndex];

        assert(commonKvp.m_key_str || commonKvp.m_key_hash);
        if (commonKvp.m_key_str)
            gameKvp.keyHash = Common::Com_HashKey(commonKvp.m_key_str->c_str(), 64);
        else
            gameKvp.keyHash = *commonKvp.m_key_hash;

        gameKvp.namespaceHash = namespaceHash;
        gameKvp.value = m_memory.Dup(commonKvp.m_value.c_str());
    }

    context.AddAsset(AssetRegistration<AssetKeyValuePairs>(m_zone.m_name, gameKvps));
}
