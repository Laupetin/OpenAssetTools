#pragma once

#include "Zone/Zone.h"
#include "Zone/Stream/IZoneInputStream.h"

class ContentLoader
{
protected:
    static const void* PTR_FOLLOWING;
    static const void* PTR_INSERT;

    const char** varXString;

    Zone* m_zone;
    IZoneInputStream* m_stream;

    ContentLoader();

    void LoadXString(bool atStreamStart) const;
    void LoadXStringArray(bool atStreamStart, size_t count) const;

public:
    virtual ~ContentLoader() = default;
};
