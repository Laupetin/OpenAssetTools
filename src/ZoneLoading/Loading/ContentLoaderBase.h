#pragma once

#include "Zone/Stream/IZoneInputStream.h"
#include "Zone/Zone.h"

class ContentLoaderBase
{
protected:
    static constexpr auto PTR_FOLLOWING = reinterpret_cast<void*>(-1);
    static constexpr auto PTR_INSERT = reinterpret_cast<void*>(-2);

public:
    virtual ~ContentLoaderBase() = default;
    ContentLoaderBase(const ContentLoaderBase& other) = default;
    ContentLoaderBase(ContentLoaderBase&& other) noexcept = default;
    ContentLoaderBase& operator=(const ContentLoaderBase& other) = delete;
    ContentLoaderBase& operator=(ContentLoaderBase&& other) noexcept = delete;

protected:
    explicit ContentLoaderBase(Zone& zone);
    ContentLoaderBase(Zone& zone, IZoneInputStream* stream);

    void LoadXString(bool atStreamStart) const;
    void LoadXStringArray(bool atStreamStart, size_t count);

    const char** varXString;

    Zone& m_zone;
    MemoryManager& m_memory;
    IZoneInputStream* m_stream;
};
