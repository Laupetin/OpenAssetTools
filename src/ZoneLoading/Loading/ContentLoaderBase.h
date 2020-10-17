#pragma once

#include "Zone/Zone.h"
#include "Zone/Stream/IZoneInputStream.h"

class ContentLoaderBase
{
protected:
    static const void* PTR_FOLLOWING;
    static const void* PTR_INSERT;

    const char** varXString;

    Zone* m_zone;
    IZoneInputStream* m_stream;

    ContentLoaderBase();

    void LoadXString(bool atStreamStart) const;
    void LoadXStringArray(bool atStreamStart, size_t count);

public:
    virtual ~ContentLoaderBase() = default;
};
