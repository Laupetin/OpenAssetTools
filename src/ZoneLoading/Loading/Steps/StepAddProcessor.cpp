#include "StepAddProcessor.h"

#include <cassert>

namespace
{
    class StepAddProcessor final : public ILoadingStep
    {
    public:
        explicit StepAddProcessor(std::unique_ptr<StreamProcessor> streamProcessor)
            : m_stream_processor(std::move(streamProcessor))
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            assert(m_stream_processor != nullptr);

            zoneLoader.AddStreamProcessor(std::move(m_stream_processor));
            m_stream_processor = nullptr;
        }

    private:
        std::unique_ptr<StreamProcessor> m_stream_processor;
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepAddProcessor(std::unique_ptr<StreamProcessor> streamProcessor)
    {
        return std::make_unique<StepAddProcessor>(std::move(streamProcessor));
    }
} // namespace step
