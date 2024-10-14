#pragma once

#include "ZoneCreation/ZoneCreator.h"

namespace T6
{
    class ZoneCreator final : public IZoneCreator
    {
    public:
        [[nodiscard]] GameId GetGameId() const override;
        [[nodiscard]] asset_type_t GetImageAssetType() const override;

    protected:
        void HandleMetadata(Zone& zone, const ZoneCreationContext& context) const override;
    };
} // namespace T6
