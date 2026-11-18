#pragma once

#include "Utils/MemoryManager.h"
#include "Zone/XBlock.h"

#include <memory>
#include <vector>

class ZoneMemory : public MemoryManager
{
public:
    ZoneMemory();

    void AddBlock(std::unique_ptr<XBlock> block);
    [[nodiscard]] size_t GetBlockCount() const;
    [[nodiscard]] XBlock* GetBlock(size_t index) const;

private:
    std::vector<std::unique_ptr<XBlock>> m_blocks;
};
