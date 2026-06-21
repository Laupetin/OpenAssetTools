#include "ImageDecompressor.h"

#include "DecompressorBc5.h"

namespace image
{
    ImageDecompressor* ImageDecompressor::GetDecompressorForFormat(const ImageFormatId formatId)
    {
        switch (formatId)
        {
        case ImageFormatId::BC1:
        case ImageFormatId::BC2:
        case ImageFormatId::BC3:
        case ImageFormatId::BC4:
            // TODO
            return nullptr;

        case ImageFormatId::BC5:
        {
            static DecompressorBc5 bc5;
            return &bc5;
        }

        default:
            return nullptr;
        }
    }
} // namespace image
