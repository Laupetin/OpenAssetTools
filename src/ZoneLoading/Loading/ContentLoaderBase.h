#pragma once

#include "Zone/Stream/IZoneInputStream.h"
#include "Zone/Zone.h"

class ContentLoaderBase
{
protected:
    static const void* PTR_FOLLOWING;
    static const void* PTR_INSERT;

    const char** varXString;

    Zone* m_zone;
    IZoneInputStream* m_stream;

    ContentLoaderBase();
    ContentLoaderBase(Zone* zone, IZoneInputStream* stream);

    void LoadXString(bool atStreamStart) const;
    void LoadXStringArray(bool atStreamStart, size_t count);

public:
    virtual ~ContentLoaderBase() = default;
};
