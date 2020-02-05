#pragma once

#include "Utils/MemoryManager.h"
#include "Zone/XBlock.h"
#include <vector>

class ZoneMemory : public MemoryManager
{
    std::vector<XBlock*> m_blocks;

public:
    ZoneMemory();
    ~ZoneMemory() override;

    void AddBlock(XBlock* block);
};
