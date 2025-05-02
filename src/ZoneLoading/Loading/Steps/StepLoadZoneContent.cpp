#include "StepLoadZoneContent.h"

#include "Zone/Stream/ZoneInputStream.h"

namespace
{
    class StepLoadZoneContent final : public ILoadingStep
    {
    public:
        StepLoadZoneContent(std::unique_ptr<IContentLoadingEntryPoint> entryPoint, const int offsetBlockBitCount, const block_t insertBlock)
            : m_content_loader(std::move(entryPoint)),
              m_offset_block_bit_count(offsetBlockBitCount),
              m_insert_block(insertBlock)
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            const auto inputStream = ZoneInputStream::Create(zoneLoader.m_blocks, stream, m_offset_block_bit_count, m_insert_block);

            m_content_loader->Load(*inputStream);
        }

    private:
        std::unique_ptr<IContentLoadingEntryPoint> m_content_loader;
        int m_offset_block_bit_count;
        block_t m_insert_block;
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep>
        CreateStepLoadZoneContent(std::unique_ptr<IContentLoadingEntryPoint> entryPoint, const int offsetBlockBitCount, const block_t insertBlock)
    {
        return std::make_unique<StepLoadZoneContent>(std::move(entryPoint), offsetBlockBitCount, insertBlock);
    }
} // namespace step
