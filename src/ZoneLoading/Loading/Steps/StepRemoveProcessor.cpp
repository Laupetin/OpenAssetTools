#include "StepRemoveProcessor.h"

#include <cassert>

namespace
{
    class StepRemoveProcessor final : public ILoadingStep
    {
    public:
        explicit StepRemoveProcessor(const StreamProcessor* streamProcessor)
            : m_stream_processor(streamProcessor)
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            assert(m_stream_processor != nullptr);

            zoneLoader.RemoveStreamProcessor(m_stream_processor);
        }

    private:
        const StreamProcessor* m_stream_processor;
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepRemoveProcessor(const StreamProcessor* streamProcessor)
    {
        return std::make_unique<StepRemoveProcessor>(streamProcessor);
    }
} // namespace step
