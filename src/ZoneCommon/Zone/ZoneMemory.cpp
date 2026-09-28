#include "ZoneMemory.h"

ZoneMemory::ZoneMemory() = default;

void ZoneMemory::AddBlock(std::unique_ptr<XBlock> block)
{
    m_blocks.emplace_back(std::move(block));
}

size_t ZoneMemory::GetBlockCount() const
{
    return m_blocks.size();
}

XBlock* ZoneMemory::GetBlock(const size_t index) const
{
    return m_blocks.at(index).get();
}
