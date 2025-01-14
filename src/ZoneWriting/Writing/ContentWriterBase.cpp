#include "ContentWriterBase.h"

#include <cassert>

ContentWriterBase::ContentWriterBase(const Zone& zone)
    : m_zone(zone),
      m_stream(nullptr),
      varXString(nullptr),
      varXStringWritten(nullptr)
{
}

ContentWriterBase::ContentWriterBase(const Zone& zone, IZoneOutputStream& stream)
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
        varXStringWritten = m_stream->Write<const char*>(varXString);
    }

    assert(varXStringWritten != nullptr);

    if (m_stream->ReusableShouldWrite(varXStringWritten))
    {
        m_stream->Align(alignof(const char));
        m_stream->ReusableAddOffset(*varXStringWritten);
        m_stream->WriteNullTerminated(*varXStringWritten);

        m_stream->MarkFollowing(*varXStringWritten);
    }
}

void ContentWriterBase::WriteXStringArray(const bool atStreamStart, const size_t count)
{
    if (atStreamStart)
    {
        assert(varXString != nullptr);
        varXStringWritten = m_stream->Write<const char*>(varXString, count);
    }

    assert(varXStringWritten != nullptr);

    for (size_t index = 0; index < count; index++)
    {
        WriteXString(false);
        varXStringWritten++;
    }
}
