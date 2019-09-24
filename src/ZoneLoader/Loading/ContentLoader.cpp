#include "ContentLoader.h"
#include <cassert>

const void* ContentLoader::PTR_FOLLOWING = reinterpret_cast<void*>(-1);
const void* ContentLoader::PTR_INSERT = reinterpret_cast<void*>(-2);

ContentLoader::ContentLoader()
{
    m_zone = nullptr;
    m_stream = nullptr;
}

void ContentLoader::LoadXString(const char** pXString) const
{
    assert(pXString != nullptr);

    if(*pXString != nullptr)
    {
        if(*pXString == PTR_FOLLOWING)
        {
            *pXString = m_stream->Alloc<const char>();
            m_stream->LoadNullTerminated();
        }
        else
        {
            *pXString = m_stream->ConvertOffsetToPointer<const char>(*pXString);
        }
    }
}

void ContentLoader::LoadXStringArray(const char** pArray, const size_t count, const bool atStreamStart) const
{
    assert(pArray != nullptr);

    if(atStreamStart)
        m_stream->Load<const char*>(count);

    for(size_t index = 0; index < count; index++)
    {
        LoadXString(&pArray[index]);
    }
}