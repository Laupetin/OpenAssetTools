#include "ZoneLoaderFactoryQOS.h"

#include "ContentLoaderQOS.h"
#include "Game/GameLanguage.h"
#include "Game/QOS/QOS.h"
#include "Game/QOS/ZoneConstantsQOS.h"
#include "Loading/Exception/InvalidXBlockSizeException.h"
#include "Loading/Exception/UnexpectedEndOfFileException.h"
#include "Loading/ILoadingStep.h"
#include "Loading/Processor/ProcessorInflate.h"
#include "Loading/Steps/StepAddProcessor.h"
#include "Loading/Steps/StepLoadZoneContent.h"
#include "Utils/ClassUtils.h"
#include "Utils/Endianness.h"

#include <array>
#include <cassert>
#include <cstring>

using namespace QOS;

namespace
{
    constexpr uint64_t MAX_XBLOCK_SIZE = 0x3C000000; // ~1GB
    constexpr size_t REMAINING_FILE_HEADER_SIZE = ZoneConstants::FILE_HEADER_SIZE - sizeof(ZoneHeader);

    static_assert(REMAINING_FILE_HEADER_SIZE == sizeof(uint32_t) * 4u);

    void SetupBlock(ZoneLoader& zoneLoader)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        zoneLoader.AddXBlock(XBLOCK_DEF(QOS::XFILE_BLOCK_TEMP, XBlockType::BLOCK_TYPE_TEMP));
        zoneLoader.AddXBlock(XBLOCK_DEF(QOS::XFILE_BLOCK_RUNTIME, XBlockType::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(QOS::XFILE_BLOCK_VIRTUAL, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(QOS::XFILE_BLOCK_LARGE, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(QOS::XFILE_BLOCK_PHYSICAL, XBlockType::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    uint32_t GetQosVersion(const ZoneHeader& header)
    {
        uint32_t version;
        static_assert(sizeof(version) <= sizeof(header.m_magic));
        std::memcpy(&version, header.m_magic, sizeof(version));
        return endianness::FromLittleEndian(version);
    }

    uint32_t GetFirstBlockSize(const ZoneHeader& header)
    {
        return endianness::FromLittleEndian(header.m_version);
    }

    class StepAllocQosXBlocks final : public ILoadingStep
    {
    public:
        explicit StepAllocQosXBlocks(const ZoneHeader& header)
            : m_first_block_size(GetFirstBlockSize(header))
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            std::array<uint32_t, 4> remainingBlockSizes{};
            if (stream.Load(remainingBlockSizes.data(), sizeof(remainingBlockSizes)) != sizeof(remainingBlockSizes))
                throw UnexpectedEndOfFileException();

            std::array<uint32_t, QOS::MAX_XFILE_COUNT> blockSizes{
                m_first_block_size,
                endianness::FromLittleEndian(remainingBlockSizes[0]),
                endianness::FromLittleEndian(remainingBlockSizes[1]),
                endianness::FromLittleEndian(remainingBlockSizes[2]),
                endianness::FromLittleEndian(remainingBlockSizes[3]),
            };

            uint64_t totalMemory = 0;
            for (const auto blockSize : blockSizes)
                totalMemory += blockSize;

            if (totalMemory > MAX_XBLOCK_SIZE)
                throw InvalidXBlockSizeException(totalMemory, MAX_XBLOCK_SIZE);

            assert(zoneLoader.m_blocks.size() == blockSizes.size());

            for (auto blockIndex = 0u; blockIndex < blockSizes.size(); blockIndex++)
                zoneLoader.m_blocks[blockIndex]->Alloc(blockSizes[blockIndex]);
        }

    private:
        uint32_t m_first_block_size;
    };
} // namespace

std::optional<ZoneLoaderInspectionResult> ZoneLoaderFactory::InspectZoneHeader(const ZoneHeader& header) const
{
    if (GetQosVersion(header) == ZoneConstants::ZONE_VERSION_PC)
    {
        return ZoneLoaderInspectionResult{
            .m_game_id = GameId::QOS,
            .m_endianness = GameEndianness::LE,
            .m_word_size = GameWordSize::ARCH_32,
            .m_platform = GamePlatform::PC,
            // There is no way to know whether unsigned zones are official.
            .m_is_official = false,
            .m_is_signed = false,
            .m_is_encrypted = false,
        };
    }

    return std::nullopt;
}

std::unique_ptr<ZoneLoader> ZoneLoaderFactory::CreateLoaderForHeader(const ZoneHeader& header,
                                                                     const std::string& fileName,
                                                                     std::optional<std::unique_ptr<ProgressCallback>> progressCallback) const
{
    const auto inspectResult = InspectZoneHeader(header);
    if (!inspectResult)
        return nullptr;

    auto zone = std::make_unique<Zone>(fileName, 0, GameId::QOS, inspectResult->m_platform);
    auto* zonePtr = zone.get();
    zone->m_language = GameLanguage::LANGUAGE_NONE;

    auto zoneLoader = std::make_unique<ZoneLoader>(std::move(zone));
    SetupBlock(*zoneLoader);
    zoneLoader->AddLoadingStep(std::make_unique<StepAllocQosXBlocks>(header));
    zoneLoader->AddLoadingStep(step::CreateStepAddProcessor(processor::CreateProcessorInflate(ZoneConstants::AUTHED_CHUNK_SIZE)));
    zoneLoader->AddLoadingStep(step::CreateStepLoadZoneContent(
        [zonePtr](ZoneInputStream& stream)
        {
            return std::make_unique<ContentLoader>(*zonePtr, stream);
        },
        32u,
        ZoneConstants::OFFSET_BLOCK_BIT_COUNT,
        ZoneConstants::INSERT_BLOCK,
        zonePtr->Memory(),
        std::move(progressCallback)));

    return zoneLoader;
}
