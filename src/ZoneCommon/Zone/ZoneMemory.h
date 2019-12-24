#pragma once

#include "Zone/XBlock.h"
#include <vector>

class ZoneMemory
{
    std::vector<XBlock*> m_blocks;
    std::vector<void*> m_allocations;

public:
    ZoneMemory();
    ~ZoneMemory();

    void AddBlock(XBlock* block);

    void* Alloc(size_t size);
    char* Dup(const char* str);
};