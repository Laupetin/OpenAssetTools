#pragma once

#include "Zone/Stream/ZoneInputStream.h"
#include "Zone/Zone.h"

#include <cstdint>

enum class ZonePointerType : std::uint8_t
{
    FOLLOWING,
    INSERT,
    OFFSET
};

class ContentLoaderBase
{
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

    [[nodiscard]] ZonePointerType GetZonePointerType(const void* zonePtr) const;

    template<typename T> [[nodiscard]] ZonePointerType GetZonePointerType(T* zonePtr) const
    {
        return GetZonePointerType(reinterpret_cast<const void*>(zonePtr));
    }

    const char** varXString;

    Zone& m_zone;
    MemoryManager& m_memory;
    ZoneInputStream& m_stream;

private:
    const void* m_zone_ptr_following;
    const void* m_zone_ptr_insert;
};
