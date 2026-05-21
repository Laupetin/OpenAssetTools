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
            stream.Skip(m_skip_count);
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
