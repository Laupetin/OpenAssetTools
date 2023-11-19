#pragma once

#include "Zone/Stream/IZoneOutputStream.h"
#include "Zone/Zone.h"

class ContentWriterBase
{
protected:
    const char** varXString;
    const char** varXStringWritten;

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
