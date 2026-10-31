#include "ZoneLoaderFactoryCODO.h"

#include "ContentLoaderCODO.h"
#include "Game/CODO/CODO.h"
#include "Game/CODO/ZoneConstantsCODO.h"
#include "Game/GameLanguage.h"
#include "Loading/Processor/ProcessorInflate.h"
#include "Loading/Steps/StepAddProcessor.h"
#include "Loading/Steps/StepAllocXBlocks.h"
#include "Loading/Steps/StepLoadZoneContent.h"
#include "Loading/Steps/StepLoadZoneSizes.h"
#include "Loading/Steps/StepSkipBytes.h"
#include "ProcessorAuthedBlocksCODO.h"
#include "Utils/ClassUtils.h"
#include "Utils/Endianness.h"

#include <cstring>

using namespace CODO;

namespace
{
    void SetupBlock(ZoneLoader& zoneLoader)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_TEMP, XBlockType::BLOCK_TYPE_TEMP));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_PHYSICAL, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_RUNTIME, XBlockType::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_VIRTUAL, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_LARGE, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_CALLBACK, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_VERTEX, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_INDEX, XBlockType::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }
} // namespace

std::optional<ZoneLoaderInspectionResult> ZoneLoaderFactory::InspectZoneHeader(ZoneDataPeeking& filePeek) const
{
    const auto& header = filePeek.PeekStruct<ZoneHeader>();
    const auto version = endianness::FromLittleEndian(header.m_version);

    if ((version == ZoneConstants::ZONE_VERSION_PC || version == ZoneConstants::ZONE_VERSION_PC_LEGACY)
        && !memcmp(header.m_magic, ZoneConstants::MAGIC_SIGNED, std::char_traits<char>::length(ZoneConstants::MAGIC_SIGNED)))
    {
        return ZoneLoaderInspectionResult{
            .m_game_id = GameId::CODO,
            .m_endianness = GameEndianness::LE,
            .m_word_size = GameWordSize::ARCH_32,
            .m_platform = GamePlatform::PC,
            .m_is_official = true,
            .m_is_signed = true,
            .m_is_encrypted = true,
        };
    }

    return std::nullopt;
}

std::unique_ptr<ZoneLoader> ZoneLoaderFactory::CreateLoaderForHeader(ZoneDataPeeking& filePeek,
                                                                     const std::string& fileName,
                                                                     std::optional<std::unique_ptr<ProgressCallback>> progressCallback) const
{
    const auto inspectResult = InspectZoneHeader(filePeek);
    if (!inspectResult)
        return nullptr;

    // Create new zone
    auto zone = std::make_unique<Zone>(fileName, 0, GameId::CODO, inspectResult->m_platform);
    auto* zonePtr = zone.get();
    zone->m_language = GameLanguage::LANGUAGE_NONE;

    // File is supported. Now setup all required steps for loading this file.
    auto zoneLoader = std::make_unique<ZoneLoader>(std::move(zone));

    SetupBlock(*zoneLoader);

    // Skip the initial header that we peeked at before
    zoneLoader->AddLoadingStep(step::CreateStepSkipBytes(sizeof(ZoneHeader)));
    zoneLoader->AddLoadingStep(step::CreateStepSkipBytes(9)); // The game ignores these bytes between the zone header and the RSA encrypted key block

    // The key of the zone is stored in an RSA encrypted block. All data following it is encrypted with it.
    auto cipherKey = std::make_shared<ZoneCipherKey>();
    zoneLoader->AddLoadingStep(CreateStepLoadZoneCipherKey(cipherKey));
    zoneLoader->AddLoadingStep(step::CreateStepAddProcessor(CreateProcessorAuthedBlocks(cipherKey)));

    zoneLoader->AddLoadingStep(step::CreateStepAddProcessor(processor::CreateProcessorInflate(ZoneConstants::AUTHED_CHUNK_SIZE)));

    // Start of the XFile struct
    zoneLoader->AddLoadingStep(step::CreateStepLoadZoneSizes());
    zoneLoader->AddLoadingStep(step::CreateStepAllocXBlocks());

    // Start of the zone content
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
