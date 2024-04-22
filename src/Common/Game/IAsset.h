#pragma once

#include "Zone/ZoneTypes.h"

struct IAssetBase
{
};

template<asset_type_t AssetTypeEnum, typename AssetType> class Asset : IAssetBase
{
public:
    static constexpr auto EnumEntry = AssetTypeEnum;
    using Type = AssetType;
};
