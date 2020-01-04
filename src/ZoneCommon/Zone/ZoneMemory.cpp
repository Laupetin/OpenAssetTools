#include "ZoneMemory.h"


ZoneMemory::AllocationInfo::AllocationInfo(IDestructible* data, void* dataPtr)
{
    m_data = data;
    m_data_ptr = dataPtr;
}

ZoneMemory::ZoneMemory()
= default;

ZoneMemory::~ZoneMemory()
{
    for (auto block : m_blocks)
    {
        delete block;
    }
    m_blocks.clear();

    for (auto allocation : m_allocations)
    {
        free(allocation);
    }
    m_allocations.clear();

    for(auto destructible : m_destructible)
    {
        delete destructible.m_data;
    }
    m_destructible.clear();
}

void ZoneMemory::AddBlock(XBlock* block)
{
    m_blocks.push_back(block);
}

void* ZoneMemory::Alloc(const size_t size)
{
    void* result = malloc(size);
    m_allocations.push_back(result);

    return result;
}

char* ZoneMemory::Dup(const char* str)
{
    char* result = _strdup(str);
    m_allocations.push_back(result);

    return result;
}

void ZoneMemory::Delete(void* data)
{
    for(auto iAlloc = m_destructible.begin(); iAlloc != m_destructible.end(); ++iAlloc)
    {
        if(iAlloc->m_data_ptr == data)
        {
            delete iAlloc->m_data;
            m_destructible.erase(iAlloc);
            return;
        }
    }
}
