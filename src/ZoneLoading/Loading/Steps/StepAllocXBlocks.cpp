#include "StepAllocXBlocks.h"

#include "Loading/Exception/InvalidXBlockSizeException.h"

namespace
{
    constexpr uint64_t MAX_XBLOCK_SIZE = 0x3C000000;

    class StepAllocXBlocks final : public ILoadingStep
    {
    public:
        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            const auto blockCount = static_cast<unsigned>(zoneLoader.m_blocks.size());

            const auto blockSizes = std::make_unique<xblock_size_t[]>(blockCount);
            stream.Load(blockSizes.get(), sizeof(xblock_size_t) * blockCount);

            uint64_t totalMemory = 0;
            for (unsigned int block = 0; block < blockCount; block++)
            {
                totalMemory += blockSizes[block];
            }

            if (totalMemory > MAX_XBLOCK_SIZE)
            {
                throw InvalidXBlockSizeException(totalMemory, MAX_XBLOCK_SIZE);
            }

            for (unsigned int block = 0; block < blockCount; block++)
            {
                zoneLoader.m_blocks[block]->Alloc(blockSizes[block]);
            }
        }
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepAllocXBlocks()
    {
        return std::make_unique<StepAllocXBlocks>();
    }
} // namespace step
