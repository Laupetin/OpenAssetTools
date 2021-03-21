#include "ContentWriterBase.h"

#include <cassert>

ContentWriterBase::ContentWriterBase()
    : varXString(nullptr),
      m_zone(nullptr),
      m_stream(nullptr)
{
}

ContentWriterBase::ContentWriterBase(Zone* zone, IZoneOutputStream* stream)
    : varXString(nullptr),
      m_zone(zone),
      m_stream(stream)
{
}

void ContentWriterBase::WriteXString(const bool atStreamStart)
{
    assert(varXString != nullptr);

    if (atStreamStart)
        varXString = m_stream->Write<const char*>(varXString);

    if (m_stream->ReusableShouldWrite(varXString))
    {
        m_stream->Align(alignof(const char));
        m_stream->ReusableAddOffset(*varXString);
        m_stream->WriteNullTerminated(*varXString);

        m_stream->MarkFollowing(*varXString);
    }
}

void ContentWriterBase::WriteXStringArray(const bool atStreamStart, const size_t count)
{
    assert(varXString != nullptr);

    if (atStreamStart)
        varXString = m_stream->Write<const char*>(varXString, count);

    for (size_t index = 0; index < count; index++)
    {
        WriteXString(false);
        varXString++;
    }
}
