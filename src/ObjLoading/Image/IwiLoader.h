#pragma once

#include "Utils/FileAPI.h"
#include "Utils/MemoryManager.h"
#include "Image/Texture.h"

class IwiLoader
{
    MemoryManager* m_memory_manager;

    static const ImageFormat* GetFormat8(int8_t format);
    Texture* LoadIwi8(FileAPI::IFile* file);

    static const ImageFormat* GetFormat27(int8_t format);
    Texture* LoadIwi27(FileAPI::IFile* file);

public:
    explicit IwiLoader(MemoryManager* memoryManager);

    Texture* LoadIwi(FileAPI::IFile* file);
};
