#include "ContentLoader.h"
#include <cassert>

const void* ContentLoader::PTR_FOLLOWING = reinterpret_cast<void*>(-1);
const void* ContentLoader::PTR_INSERT = reinterpret_cast<void*>(-2);

ContentLoader::ContentLoader()
{
    varXString = nullptr;

    m_zone = nullptr;
    m_stream = nullptr;
}

void ContentLoader::LoadXString(const bool atStreamStart) const
{
    assert(varXString != nullptr);

    if (atStreamStart)
        m_stream->Load<const char*>(varXString);

    if(*varXString != nullptr)
    {
        if(*varXString == PTR_FOLLOWING)
        {
            *varXString = m_stream->Alloc<const char>(alignof(const char));
            m_stream->LoadNullTerminated(const_cast<char*>(*varXString));
        }
        else
        {
            *varXString = m_stream->ConvertOffsetToPointer<const char>(*varXString);
        }
    }
}

void ContentLoader::LoadXStringArray(const bool atStreamStart, const size_t count)
{
    assert(varXString != nullptr);

    if(atStreamStart)
        m_stream->Load<const char*>(varXString, count);

    for(size_t index = 0; index < count; index++)
    {
        LoadXString(false);
        varXString++;
    }
}