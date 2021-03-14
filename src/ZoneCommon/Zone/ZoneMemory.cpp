#include "ZoneMemory.h"

ZoneMemory::ZoneMemory()
= default;

void ZoneMemory::AddBlock(std::unique_ptr<XBlock> block)
{
    m_blocks.emplace_back(std::move(block));
}
