#pragma once

#include "Utils/FileAPI.h"
#include "Utils/MemoryManager.h"
#include "Image/Texture.h"

class IwiLoader
{
    MemoryManager* m_memory_manager;

public:
    explicit IwiLoader(MemoryManager* memoryManager);

    Texture* LoadIwi(FileAPI::IFile* file);
};
