#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "KeyValuePairs/KeyValuePairsCreator.h"
#include "Utils/MemoryManager.h"
#include "Zone/Definition/ZoneDefinition.h"
#include "Zone/Zone.h"

namespace T6
{
    class KeyValuePairsCompiler final : public IAssetCreator
    {
    public:
        KeyValuePairsCompiler(MemoryManager& memory, const Zone& zone, const ZoneDefinition& zoneDefinition, ZoneAssetCreationStateContainer& zoneStates);

        [[nodiscard]] std::optional<asset_type_t> GetHandlingAssetType() const override;
        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override;
        void FinalizeZone(AssetCreationContext& context) override;

    private:
        MemoryManager& m_memory;
        const Zone& m_zone;
        const ZoneDefinition& m_zone_definition;
        KeyValuePairsCreator& m_kvp_creator;
    };
} // namespace T6
