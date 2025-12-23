#include "StepSkipZoneImageHeaders.h"

#include "Utils/Endianness.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace
{
    class StepSkipZoneImageHeaders final : public ILoadingStep
    {
    public:
        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {

            // Xbox fastfiles have additional header data before the auth header:
            // - 4 bytes: language flags bitmask
            // - 4 bytes: image count
            // - sizeof(XAssetStreamFile) * image count (for each language in the bitmask)
            // - 4 bytes: unknown
            // - 4 bytes: unknown
            // struct XAssetStreamFile // sizeof=0xC
            // {
            //     unsigned int fileIndex;
            //     unsigned int offset;
            //     unsigned int offsetEnd;
            // };

            uint32_t languageFlags;
            stream.Load(&languageFlags, sizeof(languageFlags));
            languageFlags = endianness::FromBigEndian(languageFlags);

            uint32_t imageCount;
            stream.Load(&imageCount, sizeof(imageCount));
            imageCount = endianness::FromBigEndian(imageCount);

            // Count how many languages are set in the bitmask
            uint32_t languageCount = 0;
            for (int i = 0; i < 15; i++)
            {
                if (languageFlags & (1 << i))
                {
                    languageCount++;
                }
            }

            // Skip image stream file data (12 bytes per image per language)
            const size_t imageDataSize = 12 * imageCount * languageCount;
            if (imageDataSize > 0)
            {
                std::vector<uint8_t> tempBuffer(std::min(imageDataSize, size_t(8192)));
                size_t skipped = 0;
                while (skipped < imageDataSize)
                {
                    const size_t toSkip = std::min(imageDataSize - skipped, tempBuffer.size());
                    stream.Load(tempBuffer.data(), toSkip);
                    skipped += toSkip;
                }
            }

            // Skip the final 8 bytes (2 unknown 4-byte values)
            uint8_t finalBytes[8];
            stream.Load(finalBytes, sizeof(finalBytes));
        }
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepSkipZoneImageHeaders()
    {
        return std::make_unique<StepSkipZoneImageHeaders>();
    }
} // namespace step
