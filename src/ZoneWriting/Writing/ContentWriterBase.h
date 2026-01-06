#pragma once

#include "Zone/Stream/ZoneOutputStream.h"
#include "Zone/Zone.h"

class ContentWriterBase
{
protected:
    explicit ContentWriterBase(const Zone& zone);
    ContentWriterBase(const Zone& zone, ZoneOutputStream& stream);

public:
    virtual ~ContentWriterBase() = default;
    ContentWriterBase(const ContentWriterBase& other) = default;
    ContentWriterBase(ContentWriterBase&& other) noexcept = default;
    ContentWriterBase& operator=(const ContentWriterBase& other) = default;
    ContentWriterBase& operator=(ContentWriterBase&& other) noexcept = default;

protected:
    void WriteXString(bool atStreamStart);
    void WriteXStringArray(bool atStreamStart, size_t count);

    const Zone& m_zone;
    ZoneOutputStream* m_stream;

    const char** varXString;
    const char** varXStringWritten;
};
