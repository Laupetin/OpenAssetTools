#pragma once

#include <memory>
#include <vector>

#include "Utils/MemoryManager.h"
#include "Zone/XBlock.h"

class ZoneMemory : public MemoryManager
{
    std::vector<std::unique_ptr<XBlock>> m_blocks;

public:
    ZoneMemory();

    void AddBlock(std::unique_ptr<XBlock> block);
};
