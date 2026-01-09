#include "ContentWriterBase.h"

#include <cassert>

ContentWriterBase::ContentWriterBase(const Zone& zone)
    : m_zone(zone),
      m_stream(nullptr),
      varXString(nullptr),
      varXStringWritten(nullptr)
{
}

ContentWriterBase::ContentWriterBase(const Zone& zone, ZoneOutputStream& stream)
    : m_zone(zone),
      m_stream(&stream),
      varXString(nullptr),
      varXStringWritten(nullptr)
{
}

void ContentWriterBase::WriteXString(const bool atStreamStart)
{
    if (atStreamStart)
    {
        assert(varXString != nullptr);
        varXStringWritten = m_stream->WriteWithFill(m_stream->GetPointerByteCount()).Offset();
    }

    assert(varXStringWritten.Offset() != nullptr);

    if (m_stream->ReusableShouldWrite(*varXString, varXStringWritten))
    {
        m_stream->Align(1);
        m_stream->ReusableAddOffset(*varXString);
        m_stream->WriteNullTerminated(*varXString);

        m_stream->MarkFollowing(varXStringWritten);
    }
}

void ContentWriterBase::WriteXStringArray(const bool atStreamStart, const size_t count)
{
    const auto pointerByteCount = m_stream->GetPointerByteCount();
    if (atStreamStart)
    {
        assert(varXString != nullptr);
        varXStringWritten = m_stream->WriteWithFill(pointerByteCount * count).Offset();
    }

    assert(varXStringWritten.Offset() != nullptr);

    for (size_t index = 0; index < count; index++)
    {
        WriteXString(false);
        varXString++;
        varXStringWritten.Inc(pointerByteCount);
    }
}
