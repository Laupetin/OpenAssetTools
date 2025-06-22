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
#include "Loading/Steps/StepLoadZoneContent.h"
#include "Loading/Steps/StepLoadZoneSizes.h"
#include "Loading/Steps/StepSkipBytes.h"
#include "Utils/ClassUtils.h"

#include <cassert>
#include <cstring>
#include <type_traits>

using namespace IW3;

namespace
{
    bool CanLoad(const ZoneHeader& header, bool* isSecure, bool* isOfficial)
    {
        assert(isSecure != nullptr);
        assert(isOfficial != nullptr);

        if (header.m_version != ZoneConstants::ZONE_VERSION)
            return false;

        if (!memcmp(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, std::char_traits<char>::length(ZoneConstants::MAGIC_UNSIGNED)))
        {
            *isSecure = false;
            *isOfficial = true;
            return true;
        }

        return false;
    }

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

std::unique_ptr<ZoneLoader> ZoneLoaderFactory::CreateLoaderForHeader(ZoneHeader& header, std::string& fileName) const
{
    bool isSecure;
    bool isOfficial;

    // Check if this file is a supported IW4 zone.
    if (!CanLoad(header, &isSecure, &isOfficial))
        return nullptr;

    // Create new zone
    auto zone = std::make_unique<Zone>(fileName, 0, IGame::GetGameById(GameId::IW3));
    auto* zonePtr = zone.get();
    zone->m_pools = std::make_unique<GameAssetPoolIW3>(zonePtr, 0);
    zone->m_language = GameLanguage::LANGUAGE_NONE;

    // File is supported. Now setup all required steps for loading this file.
    auto zoneLoader = std::make_unique<ZoneLoader>(std::move(zone));

    SetupBlock(*zoneLoader);

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
        zonePtr->Memory()));

    return zoneLoader;
}
