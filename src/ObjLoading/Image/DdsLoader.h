#pragma once

#include <istream>
#include "Utils/MemoryManager.h"
#include "Image/Texture.h"

class DdsLoader
{
    MemoryManager* m_memory_manager;

public:
    explicit DdsLoader(MemoryManager* memoryManager);

    Texture* LoadDds(std::istream& stream) const;
};
