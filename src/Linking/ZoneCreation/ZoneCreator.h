#pragma once

#include "Zone/Zone.h"
#include "ZoneCreationContext.h"

namespace zone_creator
{
    [[nodiscard]] std::unique_ptr<Zone> CreateZoneForDefinition(GameId gameId, GameVariantId variantId, ZoneCreationContext& context);
}
