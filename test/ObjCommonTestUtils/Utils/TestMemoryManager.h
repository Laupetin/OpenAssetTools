#pragma once

#include "Utils/MemoryManager.h"

#include <cstdlib>

class TestMemoryManager : public MemoryManager
{
public:
    [[nodiscard]] size_t GetAllocationCount() const
    {
        return m_allocations.size() + m_destructible.size();
    }
};
