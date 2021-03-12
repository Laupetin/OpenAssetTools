#pragma once
#include <string>

#include "Utils/ClassUtils.h"
#include "ZoneCreationContext.h"
#include "Zone/Zone.h"

class IZoneCreator
{
public:
    IZoneCreator() = default;
    virtual ~IZoneCreator() = default;
    IZoneCreator(const IZoneCreator& other) = default;
    IZoneCreator(IZoneCreator&& other) noexcept = default;
    IZoneCreator& operator=(const IZoneCreator& other) = default;
    IZoneCreator& operator=(IZoneCreator&& other) noexcept = default;

    _NODISCARD virtual bool SupportsGame(const std::string& gameName) const = 0;
    _NODISCARD virtual std::unique_ptr<Zone> CreateZoneForDefinition(ZoneCreationContext& context) const = 0;
};
