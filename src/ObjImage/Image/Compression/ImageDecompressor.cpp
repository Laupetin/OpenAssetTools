#include "ImageDecompressor.h"

#include "DecompressorBc1.h"
#include "DecompressorBc2.h"
#include "DecompressorBc3.h"
#include "DecompressorBc4.h"
#include "DecompressorBc5.h"

namespace image
{
    ImageDecompressor* ImageDecompressor::GetDecompressorForFormat(const ImageFormatId formatId)
    {
        switch (formatId)
        {
        case ImageFormatId::BC1:
        {
            static DecompressorBc1 bc1;
            return &bc1;
        }

        case ImageFormatId::BC2:
        {
            static DecompressorBc2 bc2;
            return &bc2;
        }

        case ImageFormatId::BC3:
        {
            static DecompressorBc3 bc3;
            return &bc3;
        }

        case ImageFormatId::BC4:
        {
            static DecompressorBc4 bc4;
            return &bc4;
        }

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
