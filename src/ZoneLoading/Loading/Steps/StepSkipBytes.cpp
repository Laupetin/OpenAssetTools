#include "StepSkipBytes.h"

namespace
{
    class StepSkipBytes final : public ILoadingStep
    {
    public:
        explicit StepSkipBytes(const size_t skipCount)
            : m_skip_count(skipCount)
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            uint8_t tempBuffer[128];
            auto skippedBytes = 0uz;

            while (skippedBytes < m_skip_count)
            {
                size_t toSkip;

                if (m_skip_count - skippedBytes < sizeof(tempBuffer))
                {
                    toSkip = m_skip_count - skippedBytes;
                }
                else
                {
                    toSkip = sizeof(tempBuffer);
                }

                stream.Load(tempBuffer, toSkip);

                skippedBytes += toSkip;
            }
        }

    private:
        size_t m_skip_count;
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepSkipBytes(size_t skipCount)
    {
        return std::make_unique<StepSkipBytes>(skipCount);
    }
} // namespace step
