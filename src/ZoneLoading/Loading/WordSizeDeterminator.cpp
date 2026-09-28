#include "WordSizeDeterminator.h"

#include <cstdint>

namespace
{
    enum class WordSizeConfidence : std::uint8_t
    {
        UNLIKELY,
        POSSIBLE,
        REALISTIC
    };

    template<std::integral TypeCount, std::integral TypePointer>
    [[nodiscard]] WordSizeConfidence GiveConfidenceOnWordSize(const char* data, const XAssetListOffsets& offsets)
    {
        constexpr TypePointer pointerZero = 0;
        constexpr TypePointer pointerMinusOne = std::numeric_limits<TypePointer>::max();

        const auto stringCount = *reinterpret_cast<const TypeCount*>(data + offsets.offsetStringCount);
        const auto stringPointer = *reinterpret_cast<const TypePointer*>(data + offsets.offsetStringPointer);
        const auto assetCount = *reinterpret_cast<const TypeCount*>(data + offsets.offsetAssetCount);
        const auto assetPointer = *reinterpret_cast<const TypePointer*>(data + offsets.offsetAssetPointer);

        // If one of the pointers is not zero or minus one this is not standard
        if (stringPointer != pointerZero && stringPointer != pointerMinusOne)
            return WordSizeConfidence::UNLIKELY;
        if (assetPointer != pointerZero && assetPointer != pointerMinusOne)
            return WordSizeConfidence::UNLIKELY;

        // If one of the pointers is zero but the count is not zero this would in theory be valid
        // but no vanilla or OAT would look like this.
        if (stringPointer == pointerZero && stringCount > 0)
            return WordSizeConfidence::POSSIBLE;
        if (assetPointer == pointerZero && assetCount > 0)
            return WordSizeConfidence::POSSIBLE;

        // This seems plausible
        return WordSizeConfidence::REALISTIC;
    }

    class StepDetermineWordSize : public ILoadingStep
    {
    public:
        StepDetermineWordSize(const XAssetListOffsets& offsets32, const XAssetListOffsets& offsets64)
            : m_offsets_32(offsets32),
              m_offsets_64(offsets64)
        {
        }

        void PerformStep(ZoneReader& zoneReader, ILoadingStream& stream) override
        {
            std::vector<char> data(GetMaxRequiredReadSize());
            if (stream.Load(data.data(), data.size()) < data.size())
                return;

            const auto confidence32 = GiveConfidenceOnWordSize<uint32_t, uint32_t>(data.data(), m_offsets_32);
            const auto confidence64 = GiveConfidenceOnWordSize<uint32_t, uint64_t>(data.data(), m_offsets_64);

            if (confidence64 == WordSizeConfidence::REALISTIC || confidence32 == WordSizeConfidence::REALISTIC)
                m_result = confidence64 == WordSizeConfidence::REALISTIC ? GameWordSize::ARCH_64 : GameWordSize::ARCH_32;
            else if (confidence64 == WordSizeConfidence::POSSIBLE)
                m_result = GameWordSize::ARCH_64;
            else if (confidence32 == WordSizeConfidence::POSSIBLE)
                m_result = GameWordSize::ARCH_32;
            else
                m_result = std::nullopt;
        }

        [[nodiscard]] std::optional<GameWordSize> GetResult() const
        {
            return m_result;
        }

    private:
        [[nodiscard]] size_t GetMaxRequiredReadSize() const
        {
            return std::max({
                m_offsets_32.offsetStringCount + 4,
                m_offsets_32.offsetStringPointer + 4,
                m_offsets_32.offsetAssetCount + 4,
                m_offsets_32.offsetAssetPointer + 4,
                m_offsets_64.offsetStringCount + 4,
                m_offsets_64.offsetStringPointer + 8,
                m_offsets_64.offsetAssetCount + 4,
                m_offsets_64.offsetAssetPointer + 8,
            });
        }

        std::optional<GameWordSize> m_result;
        XAssetListOffsets m_offsets_32;
        XAssetListOffsets m_offsets_64;
    };
} // namespace

std::optional<GameWordSize> DetermineWordSizeFromXAssetList(std::istream& stream,
                                                            std::unique_ptr<ZoneReader> zoneReader,
                                                            const XAssetListOffsets& offsets32,
                                                            const XAssetListOffsets& offsets64)
{
    auto determinationStep = std::make_unique<StepDetermineWordSize>(offsets32, offsets64);
    const auto* determinationStepPtr = determinationStep.get();
    zoneReader->AddLoadingStep(std::move(determinationStep));
    if (!zoneReader->Run(stream))
        return std::nullopt;

    return determinationStepPtr->GetResult();
}
