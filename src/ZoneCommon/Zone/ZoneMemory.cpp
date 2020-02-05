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
}

void ZoneMemory::AddBlock(XBlock* block)
{
    m_blocks.push_back(block);
}
