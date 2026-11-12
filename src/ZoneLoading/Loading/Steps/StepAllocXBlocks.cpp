#include "StepAllocXBlocks.h"

#include "Loading/Exception/InvalidXBlockSizeException.h"

namespace
{
    constexpr uint64_t MAX_XBLOCK_SIZE = 0x3C000000; // ~1GB

    class StepAllocXBlocks final : public ILoadingStep
    {
    public:
        explicit StepAllocXBlocks(Zone* zone)
            : m_zone(zone)
        {
        }

        void PerformStep(ZoneReader& zoneReader, ILoadingStream& stream) override
        {
            const auto& zoneMemory = m_zone->Memory();
            const auto blockCount = static_cast<unsigned>(zoneMemory.GetBlockCount());

            std::vector<xblock_size_t> blockSizes(blockCount);
            stream.Load(blockSizes.data(), sizeof(xblock_size_t) * blockCount);

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
                zoneMemory.GetBlock(block)->Alloc(blockSizes[block]);
            }
        }

    private:
        Zone* m_zone;
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepAllocXBlocks(Zone* zone)
    {
        return std::make_unique<StepAllocXBlocks>(zone);
    }
} // namespace step
