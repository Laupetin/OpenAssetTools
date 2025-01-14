#pragma once

#include "Zone/Stream/IZoneOutputStream.h"

class IContentWritingEntryPoint
{
public:
    IContentWritingEntryPoint() = default;
    virtual ~IContentWritingEntryPoint() = default;
    IContentWritingEntryPoint(const IContentWritingEntryPoint& other) = default;
    IContentWritingEntryPoint(IContentWritingEntryPoint&& other) noexcept = default;
    IContentWritingEntryPoint& operator=(const IContentWritingEntryPoint& other) = default;
    IContentWritingEntryPoint& operator=(IContentWritingEntryPoint&& other) noexcept = default;

    virtual void WriteContent(IZoneOutputStream& stream) = 0;
};
