#pragma once
#include "Utils/ClassUtils.h"
#include "Zone/Zone.h"
#include "ZoneCreationContext.h"

#include <string>

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
