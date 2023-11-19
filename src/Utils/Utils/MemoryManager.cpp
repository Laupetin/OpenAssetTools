#include "MemoryManager.h"

#include <cstdlib>
#include <cstring>

MemoryManager::AllocationInfo::AllocationInfo(IDestructible* data, void* dataPtr)
{
    m_data = data;
    m_data_ptr = dataPtr;
}

MemoryManager::MemoryManager() = default;

MemoryManager::~MemoryManager()
{
    for (auto allocation : m_allocations)
    {
        free(allocation);
    }
    m_allocations.clear();

    for (auto destructible : m_destructible)
    {
        delete destructible.m_data;
    }
    m_destructible.clear();
}

void* MemoryManager::Alloc(const size_t size)
{
    void* result = malloc(size);
    m_allocations.push_back(result);

    return result;
}

char* MemoryManager::Dup(const char* str)
{
#ifdef _MSC_VER
    auto* result = _strdup(str);
#else
    auto* result = strdup(str);
#endif
    m_allocations.push_back(result);

    return result;
}

void MemoryManager::Free(void* data)
{
    for (auto iAlloc = m_allocations.begin(); iAlloc != m_allocations.end(); ++iAlloc)
    {
        if (*iAlloc == data)
        {
            free(*iAlloc);
            m_allocations.erase(iAlloc);
            return;
        }
    }
}

void MemoryManager::Delete(void* data)
{
    for (auto iAlloc = m_destructible.begin(); iAlloc != m_destructible.end(); ++iAlloc)
    {
        if (iAlloc->m_data_ptr == data)
        {
            delete iAlloc->m_data;
            m_destructible.erase(iAlloc);
            return;
        }
    }
}
