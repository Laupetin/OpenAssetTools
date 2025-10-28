#pragma once

#include "Game/CommonAsset.h"

namespace IW4
{
    class CommonAssetTypeMapper final : public ICommonAssetTypeMapper
    {
    public:
        CommonAssetTypeMapper();

        [[nodiscard]] CommonAssetType GameToCommonAssetType(asset_type_t gameAssetType) const override;
        [[nodiscard]] std::optional<asset_type_t> CommonToGameAssetType(CommonAssetType commonAssetType) const override;
    };
} // namespace IW4
