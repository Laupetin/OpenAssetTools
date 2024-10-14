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
    [[nodiscard]] virtual asset_type_t GetImageAssetType() const = 0;

    [[nodiscard]] std::unique_ptr<Zone> CreateZoneForDefinition(ZoneCreationContext& context) const;

    static const IZoneCreator* GetCreatorForGame(GameId game);

protected:
    virtual void HandleMetadata(Zone& zone, const ZoneCreationContext& context) const {}
};
