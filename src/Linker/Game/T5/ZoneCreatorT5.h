#pragma once

#include "ZoneCreation/ZoneCreator.h"

namespace T5
{
    class ZoneCreator final : public IZoneCreator
    {
    public:
        [[nodiscard]] GameId GetGameId() const override;
        [[nodiscard]] asset_type_t GetImageAssetType() const override;
    };
} // namespace T5
