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
                            MemoryManager& memory,
                            std::optional<std::unique_ptr<ProgressCallback>> progressCallback)
            : m_entry_point_factory(std::move(entryPointFactory)),
              m_pointer_bit_count(pointerBitCount),
              m_offset_block_bit_count(offsetBlockBitCount),
              m_insert_block(insertBlock),
              m_memory(memory),
              m_progress_callback(std::move(progressCallback))
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            const auto inputStream = ZoneInputStream::Create(
                m_pointer_bit_count, m_offset_block_bit_count, zoneLoader.m_blocks, m_insert_block, stream, m_memory, std::move(m_progress_callback));

            const auto entryPoint = m_entry_point_factory(*inputStream);
            assert(entryPoint);

            entryPoint->Load();
        }

    private:
        std::function<std::unique_ptr<IContentLoadingEntryPoint>(ZoneInputStream&)> m_entry_point_factory;
        unsigned m_pointer_bit_count;
        unsigned m_offset_block_bit_count;
        block_t m_insert_block;
        MemoryManager& m_memory;
        std::optional<std::unique_ptr<ProgressCallback>> m_progress_callback;
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepLoadZoneContent(std::function<std::unique_ptr<IContentLoadingEntryPoint>(ZoneInputStream&)> entryPointFactory,
                                                            const unsigned pointerBitCount,
                                                            const unsigned offsetBlockBitCount,
                                                            const block_t insertBlock,
                                                            MemoryManager& memory,
                                                            std::optional<std::unique_ptr<ProgressCallback>> progressCallback)
    {
        return std::make_unique<StepLoadZoneContent>(
            std::move(entryPointFactory), pointerBitCount, offsetBlockBitCount, insertBlock, memory, std::move(progressCallback));
    }
} // namespace step
