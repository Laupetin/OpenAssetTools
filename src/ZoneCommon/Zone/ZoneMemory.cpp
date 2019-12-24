#include "ZoneMemory.h"

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
