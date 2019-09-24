#pragma once

#include "Zone/Zone.h"
#include "Zone/Stream/IZoneInputStream.h"

class ContentLoader
{
protected:
    static const void* PTR_FOLLOWING;
    static const void* PTR_INSERT;

    Zone* m_zone;
    IZoneInputStream* m_stream;

    ContentLoader();

    void LoadXString(const char** pXString) const;
    void LoadXStringArray(const char** pArray, size_t count, bool atStreamStart) const;

public:
    virtual ~ContentLoader() = default;
};
