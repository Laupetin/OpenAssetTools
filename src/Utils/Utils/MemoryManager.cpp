#include "MemoryManager.h"

#include <cstdlib>
#include <cstring>

MemoryManager::MemoryManager() = default;

MemoryManager::~MemoryManager()
{
    for (auto* allocation : m_allocations)
        free(allocation);

    m_allocations.clear();
}

void* MemoryManager::AllocRaw(const size_t size)
{
    void* result = calloc(size, 1u);
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

void MemoryManager::Free(const void* data)
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
