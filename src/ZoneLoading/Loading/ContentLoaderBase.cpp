#include "ContentLoaderBase.h"

#include <cassert>

const void* ContentLoaderBase::PTR_FOLLOWING = reinterpret_cast<void*>(-1);
const void* ContentLoaderBase::PTR_INSERT = reinterpret_cast<void*>(-2);

ContentLoaderBase::ContentLoaderBase(Zone& zone, ZoneInputStream& stream)
    : varXString(nullptr),
      m_zone(zone),
      m_memory(zone.Memory()),
      m_stream(stream)
{
}

void ContentLoaderBase::LoadXString(const bool atStreamStart) const
{
    assert(varXString != nullptr);

    if (atStreamStart)
        m_stream.Load<const char*>(varXString);

    if (*varXString != nullptr)
    {
        if (*varXString == PTR_FOLLOWING)
        {
            *varXString = m_stream.Alloc<const char>(alignof(const char));
            m_stream.LoadNullTerminated(const_cast<char*>(*varXString));
        }
        else
        {
            *varXString = m_stream.ConvertOffsetToPointerNative<const char>(*varXString);
        }
    }
}

void ContentLoaderBase::LoadXStringArray(const bool atStreamStart, const size_t count)
{
    assert(varXString != nullptr);

    if (atStreamStart)
        m_stream.Load<const char*>(varXString, count);

    for (size_t index = 0; index < count; index++)
    {
        LoadXString(false);
        varXString++;
    }
}
