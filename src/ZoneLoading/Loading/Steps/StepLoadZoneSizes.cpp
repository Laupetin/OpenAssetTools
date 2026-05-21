#include "StepLoadZoneSizes.h"

namespace
{
    class StepLoadZoneSizes final : public step::IStepLoadZoneSizes
    {
    public:
        StepLoadZoneSizes()
            : m_size(0),
              m_external_size(0)
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            stream.Load(&m_size, sizeof(m_size));
            stream.Load(&m_external_size, sizeof(m_external_size));
        }

#ifdef ARCH_86
        [[nodiscard]] uint32_t GetSize() const override
#else
        [[nodiscard]] uint64_t GetSize() const override
#endif
        {
            return m_size;
        }

#ifdef ARCH_86
        [[nodiscard]] uint32_t GetExternalSize() const override
#else
        [[nodiscard]] uint64_t GetExternalSize() const override
#endif
        {
            return m_external_size;
        }

    private:
#ifdef ARCH_86
        uint32_t m_size;
        uint32_t m_external_size;
#else
        uint64_t m_size;
        uint64_t m_external_size;
#endif
    };
} // namespace

namespace step
{
    std::unique_ptr<IStepLoadZoneSizes> CreateStepLoadZoneSizes()
    {
        return std::make_unique<StepLoadZoneSizes>();
    }
} // namespace step
