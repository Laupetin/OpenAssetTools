#pragma once

#include "Zone/Stream/ZoneInputStream.h"
#include "Zone/Zone.h"

class ContentLoaderBase
{
protected:
    static const void* PTR_FOLLOWING;
    static const void* PTR_INSERT;

public:
    virtual ~ContentLoaderBase() = default;
    ContentLoaderBase(const ContentLoaderBase& other) = default;
    ContentLoaderBase(ContentLoaderBase&& other) noexcept = default;
    ContentLoaderBase& operator=(const ContentLoaderBase& other) = delete;
    ContentLoaderBase& operator=(ContentLoaderBase&& other) noexcept = delete;

protected:
    ContentLoaderBase(Zone& zone, ZoneInputStream& stream);

    void LoadXString(bool atStreamStart) const;
    void LoadXStringArray(bool atStreamStart, size_t count);

    const char** varXString;

    Zone& m_zone;
    MemoryManager& m_memory;
    ZoneInputStream* m_stream; // TODO: Change this to reference
};
