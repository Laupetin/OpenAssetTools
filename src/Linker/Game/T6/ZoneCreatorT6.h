#pragma once
#include "ZoneCreation/IZoneCreator.h"

namespace T6
{
    class ZoneCreator final : public IZoneCreator
    {
    public:
        _NODISCARD bool SupportsGame(const std::string& gameName) const override;
        _NODISCARD std::unique_ptr<Zone> CreateZoneForDefinition(ZoneCreationContext& context) const override;
    };
}
