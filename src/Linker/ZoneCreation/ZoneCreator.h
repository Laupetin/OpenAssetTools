#pragma once

#include "Zone/Zone.h"
#include "ZoneCreationContext.h"

class IZoneCreator
{
public:
    IZoneCreator() = default;
    virtual ~IZoneCreator() = default;
    IZoneCreator(const IZoneCreator& other) = default;
    IZoneCreator(IZoneCreator&& other) noexcept = default;
    IZoneCreator& operator=(const IZoneCreator& other) = default;
    IZoneCreator& operator=(IZoneCreator&& other) noexcept = default;

    [[nodiscard]] virtual GameId GetGameId() const = 0;
    [[nodiscard]] virtual std::unique_ptr<Zone> CreateZoneForDefinition(ZoneCreationContext& context) const = 0;
    [[nodiscard]] virtual asset_type_t GetImageAssetType() const = 0;

    static const IZoneCreator* GetCreatorForGame(GameId game);
};
