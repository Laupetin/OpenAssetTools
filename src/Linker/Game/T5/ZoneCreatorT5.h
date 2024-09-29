#pragma once
#include "AssetLoading/AssetLoadingContext.h"
#include "Zone/ZoneTypes.h"
#include "ZoneCreation/ZoneCreator.h"

namespace T5
{
    class ZoneCreator final : public IZoneCreator
    {
        static std::vector<Gdt*> CreateGdtList(ZoneCreationContext& context);
        static void ApplyIgnoredAssets(const ZoneCreationContext& creationContext, AssetLoadingContext& loadingContext);
        void CreateZoneAssetPools(Zone* zone) const;

    public:
        [[nodiscard]] GameId GetGameId() const override;
        [[nodiscard]] std::unique_ptr<Zone> CreateZoneForDefinition(ZoneCreationContext& context) const override;
        [[nodiscard]] asset_type_t GetImageAssetType() const override;
    };
} // namespace T5
