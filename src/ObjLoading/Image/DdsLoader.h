#pragma once

#include "Image/Texture.h"
#include "Utils/MemoryManager.h"

#include <istream>

class DdsLoader
{
    MemoryManager* m_memory_manager;

public:
    explicit DdsLoader(MemoryManager* memoryManager);

    Texture* LoadDds(std::istream& stream) const;
};
