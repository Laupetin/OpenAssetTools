#pragma once

#include "ZoneCreation/ZoneCreator.h"

namespace IW4
{
    class ZoneCreator final : public IZoneCreator
    {
    public:
        [[nodiscard]] GameId GetGameId() const override;
        [[nodiscard]] asset_type_t GetImageAssetType() const override;
    };
} // namespace IW4
