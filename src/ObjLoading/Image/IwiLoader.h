#pragma once

#include "Image/Texture.h"
#include "Utils/MemoryManager.h"

#include <istream>

class IwiLoader
{
    MemoryManager* m_memory_manager;

    static const ImageFormat* GetFormat6(int8_t format);
    Texture* LoadIwi6(std::istream& stream) const;

    static const ImageFormat* GetFormat8(int8_t format);
    Texture* LoadIwi8(std::istream& stream) const;

    static const ImageFormat* GetFormat13(int8_t format);
    Texture* LoadIwi13(std::istream& stream) const;

    static const ImageFormat* GetFormat27(int8_t format);
    Texture* LoadIwi27(std::istream& stream) const;

public:
    explicit IwiLoader(MemoryManager* memoryManager);

    Texture* LoadIwi(std::istream& stream);
};
