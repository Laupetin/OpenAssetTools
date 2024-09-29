#pragma once

#include "AssetLoading/AssetLoadingContext.h"
#include "Zone/ZoneTypes.h"
#include "ZoneCreation/ZoneCreator.h"

namespace IW3
{
    class ZoneCreator final : public IZoneCreator
    {
        static std::vector<Gdt*> CreateGdtList(const ZoneCreationContext& context);
        static void ApplyIgnoredAssets(const ZoneCreationContext& creationContext, AssetLoadingContext& loadingContext);
        void CreateZoneAssetPools(Zone* zone) const;

    public:
        [[nodiscard]] GameId GetGameId() const override;
        [[nodiscard]] std::unique_ptr<Zone> CreateZoneForDefinition(ZoneCreationContext& context) const override;
        [[nodiscard]] asset_type_t GetImageAssetType() const override;
    };
} // namespace IW3
