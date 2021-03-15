#pragma once

#include "Zone/Zone.h"
#include "Zone/Stream/IZoneOutputStream.h"

class ContentWriterBase
{
protected:
    static constexpr void* PTR_FOLLOWING = reinterpret_cast<void*>(-1);
    static constexpr void* PTR_INSERT = reinterpret_cast<void*>(-2);

    const char** varXString;

    Zone* m_zone;
    IZoneOutputStream* m_stream;

    ContentWriterBase();
    ContentWriterBase(Zone* zone, IZoneOutputStream* stream);

    void WriteXString(bool atStreamStart) const;
    void WriteXStringArray(bool atStreamStart, size_t count);

public:
    virtual ~ContentWriterBase() = default;
    ContentWriterBase(const ContentWriterBase& other) = default;
    ContentWriterBase(ContentWriterBase&& other) noexcept = default;
    ContentWriterBase& operator=(const ContentWriterBase& other) = default;
    ContentWriterBase& operator=(ContentWriterBase&& other) noexcept = default;
};
