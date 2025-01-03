#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "Zone/Definition/ZoneDefinition.h"

namespace T6
{
    class KeyValuePairsCompiler final : public IAssetCreator
    {
    public:
        KeyValuePairsCompiler(const Zone& zone, const ZoneDefinition& zoneDefinition);

        [[nodiscard]] std::optional<asset_type_t> GetHandlingAssetType() const override;
        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override;
        void FinalizeZone(AssetCreationContext& context) override;

    private:
        const Zone& m_zone;
        const ZoneDefinition& m_zone_definition;
    };
} // namespace T6
