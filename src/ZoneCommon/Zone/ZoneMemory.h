#pragma once

#include "Utils/MemoryManager.h"
#include "Zone/XBlock.h"

#include <memory>
#include <vector>

class ZoneMemory : public MemoryManager
{
    std::vector<std::unique_ptr<XBlock>> m_blocks;

public:
    ZoneMemory();

    void AddBlock(std::unique_ptr<XBlock> block);
};
