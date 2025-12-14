#include "ZoneLoaderFactoryIW3.h"

#include "ContentLoaderIW3.h"
#include "Game/GameLanguage.h"
#include "Game/IW3/GameAssetPoolIW3.h"
#include "Game/IW3/GameIW3.h"
#include "Game/IW3/IW3.h"
#include "Game/IW3/ZoneConstantsIW3.h"
#include "Loading/Processor/ProcessorInflate.h"
#include "Loading/Steps/StepAddProcessor.h"
#include "Loading/Steps/StepAllocXBlocks.h"
#include "Loading/Steps/StepDumpData.h"
#include "Loading/Steps/StepLoadZoneContent.h"
#include "Loading/Steps/StepLoadZoneSizes.h"
#include "Utils/ClassUtils.h"
#include "Utils/Endianness.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <cstring>
#include <filesystem>
#include <type_traits>

using namespace IW3;
namespace fs = std::filesystem;

namespace
{
    void SetupBlock(ZoneLoader& zoneLoader)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        zoneLoader.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_TEMP, XBlockType::BLOCK_TYPE_TEMP));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_RUNTIME, XBlockType::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_LARGE_RUNTIME, XBlockType::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_PHYSICAL_RUNTIME, XBlockType::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_VIRTUAL, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_LARGE, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_PHYSICAL, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_VERTEX, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_INDEX, XBlockType::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }
} // namespace

std::optional<ZoneLoaderInspectionResult> ZoneLoaderFactory::InspectZoneHeader(const ZoneHeader& header) const
{
    if (endianness::FromLittleEndian(header.m_version) == ZoneConstants::ZONE_VERSION_PC)
    {
        if (!memcmp(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, std::char_traits<char>::length(ZoneConstants::MAGIC_UNSIGNED)))
        {
            return ZoneLoaderInspectionResult{
                .m_game_id = GameId::IW3,
                .m_endianness = GameEndianness::LE,
                .m_word_size = GameWordSize::ARCH_32,
                .m_platform = GamePlatform::PC,
                .m_is_official = true,
                .m_is_signed = false,
                .m_is_encrypted = false,
            };
        }
    }
    else if (endianness::FromBigEndian(header.m_version) == ZoneConstants::ZONE_VERSION_XENON)
    {
        if (!memcmp(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, std::char_traits<char>::length(ZoneConstants::MAGIC_UNSIGNED)))
        {
            return ZoneLoaderInspectionResult{
                .m_game_id = GameId::IW3,
                .m_endianness = GameEndianness::BE,
                .m_word_size = GameWordSize::ARCH_32,
                .m_platform = GamePlatform::XBOX,
                .m_is_official = false,
                .m_is_signed = false,
                .m_is_encrypted = false,
            };
        }
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

    // Create new zone
    auto zone = std::make_unique<Zone>(fileName, 0, GameId::IW3, inspectResult->m_platform);
    auto* zonePtr = zone.get();
    zone->m_pools = std::make_unique<GameAssetPoolIW3>(zonePtr, 0);
    zone->m_language = GameLanguage::LANGUAGE_NONE;

    // File is supported. Now setup all required steps for loading this file.
    auto zoneLoader = std::make_unique<ZoneLoader>(std::move(zone));

    SetupBlock(*zoneLoader);

    zoneLoader->AddLoadingStep(step::CreateStepAddProcessor(processor::CreateProcessorInflate(ZoneConstants::AUTHED_CHUNK_SIZE)));

    if (inspectResult->m_endianness == GameEndianness::LE)
    {
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
    }
    else
    {
        fs::path dumpFileNamePath = fs::path(fileName).filename();
        dumpFileNamePath.replace_extension(".dat");
        std::string dumpFileName = dumpFileNamePath.string();
        con::warn("Dumping xbox assets is not supported, making a full fastfile data dump to {}", dumpFileName);
        zoneLoader->AddLoadingStep(step::CreateStepDumpData(dumpFileName, 0xFFFFFFFF));
    }

    return zoneLoader;
}
