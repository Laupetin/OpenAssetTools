#include "StepLoadZoneContent.h"

#include "Zone/Stream/ZoneInputStream.h"

namespace
{
    class StepLoadZoneContent final : public ILoadingStep
    {
    public:
        StepLoadZoneContent(std::function<std::unique_ptr<IContentLoadingEntryPoint>(ZoneInputStream&)> entryPointFactory,
                            const unsigned pointerBitCount,
                            const unsigned offsetBlockBitCount,
                            const block_t insertBlock,
                            Zone* zone,
                            std::optional<std::unique_ptr<ProgressCallback>> progressCallback)
            : m_entry_point_factory(std::move(entryPointFactory)),
              m_pointer_bit_count(pointerBitCount),
              m_offset_block_bit_count(offsetBlockBitCount),
              m_insert_block(insertBlock),
              m_zone(zone),
              m_progress_callback(std::move(progressCallback))
        {
        }

        void PerformStep(ZoneReader& zoneReader, ILoadingStream& stream) override
        {
            const auto& zoneMemory = m_zone->Memory();
            const auto blockCount = zoneMemory.GetBlockCount();
            m_blocks = std::vector<XBlock*>();
            m_blocks.reserve(blockCount);
            for (size_t i = 0; i < blockCount; ++i)
                m_blocks.emplace_back(zoneMemory.GetBlock(i));

            const auto inputStream = ZoneInputStream::Create(
                m_pointer_bit_count, m_offset_block_bit_count, m_blocks, m_insert_block, stream, m_zone->Memory(), std::move(m_progress_callback));

            const auto entryPoint = m_entry_point_factory(*inputStream);
            assert(entryPoint);

            entryPoint->Load();
        }

    private:
        std::function<std::unique_ptr<IContentLoadingEntryPoint>(ZoneInputStream&)> m_entry_point_factory;
        unsigned m_pointer_bit_count;
        unsigned m_offset_block_bit_count;
        block_t m_insert_block;
        Zone* m_zone;
        std::vector<XBlock*> m_blocks;
        std::optional<std::unique_ptr<ProgressCallback>> m_progress_callback;
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepLoadZoneContent(std::function<std::unique_ptr<IContentLoadingEntryPoint>(ZoneInputStream&)> entryPointFactory,
                                                            const unsigned pointerBitCount,
                                                            const unsigned offsetBlockBitCount,
                                                            const block_t insertBlock,
                                                            Zone* zone,
                                                            std::optional<std::unique_ptr<ProgressCallback>> progressCallback)
    {
        return std::make_unique<StepLoadZoneContent>(
            std::move(entryPointFactory), pointerBitCount, offsetBlockBitCount, insertBlock, zone, std::move(progressCallback));
    }
} // namespace step
