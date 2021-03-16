#pragma once

#include "Zone/Zone.h"
#include "Zone/Stream/IZoneOutputStream.h"

class ContentWriterBase
{
protected:
    const char** varXString;

    Zone* m_zone;
    IZoneOutputStream* m_stream;

    ContentWriterBase();
    ContentWriterBase(Zone* zone, IZoneOutputStream* stream);

    void WriteXString(bool atStreamStart);
    void WriteXStringArray(bool atStreamStart, size_t count);

public:
    virtual ~ContentWriterBase() = default;
    ContentWriterBase(const ContentWriterBase& other) = default;
    ContentWriterBase(ContentWriterBase&& other) noexcept = default;
    ContentWriterBase& operator=(const ContentWriterBase& other) = default;
    ContentWriterBase& operator=(ContentWriterBase&& other) noexcept = default;
};
