#include "ZoneLoaderFactoryT5.h"

#include "ContentLoaderT5.h"
#include "Game/GameLanguage.h"
#include "Game/T5/GameAssetPoolT5.h"
#include "Game/T5/GameT5.h"
#include "Game/T5/T5.h"
#include "Game/T5/ZoneConstantsT5.h"
#include "Loading/Processor/ProcessorInflate.h"
#include "Loading/Steps/StepAddProcessor.h"
#include "Loading/Steps/StepAllocXBlocks.h"
#include "Loading/Steps/StepLoadZoneContent.h"
#include "Loading/Steps/StepSkipBytes.h"
#include "Utils/ClassUtils.h"

#include <cassert>
#include <cstring>
#include <type_traits>

using namespace T5;

namespace
{
    bool CanLoad(const ZoneHeader& header, bool* isSecure, bool* isOfficial)
    {
        assert(isSecure != nullptr);
        assert(isOfficial != nullptr);

        if (header.m_version != ZoneConstants::ZONE_VERSION)
        {
            return false;
        }

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

        zoneLoader.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_TEMP, XBlock::Type::BLOCK_TYPE_TEMP));
        zoneLoader.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_LARGE_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_PHYSICAL_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_VIRTUAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_LARGE, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_PHYSICAL, XBlock::Type::BLOCK_TYPE_NORMAL));

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
    auto zone = std::make_unique<Zone>(fileName, 0, IGame::GetGameById(GameId::T5));
    auto* zonePtr = zone.get();
    zone->m_pools = std::make_unique<GameAssetPoolT5>(zonePtr, 0);
    zone->m_language = GameLanguage::LANGUAGE_NONE;

    // File is supported. Now setup all required steps for loading this file.
    auto zoneLoader = std::make_unique<ZoneLoader>(std::move(zone));

    SetupBlock(*zoneLoader);

    zoneLoader->AddLoadingStep(std::make_unique<StepAddProcessor>(std::make_unique<ProcessorInflate>(ZoneConstants::AUTHED_CHUNK_SIZE)));

    // Start of the XFile struct
    zoneLoader->AddLoadingStep(std::make_unique<StepSkipBytes>(8));
    // Skip size and externalSize fields since they are not interesting for us
    zoneLoader->AddLoadingStep(std::make_unique<StepAllocXBlocks>());

    // Start of the zone content
    zoneLoader->AddLoadingStep(
        std::make_unique<StepLoadZoneContent>(std::make_unique<ContentLoader>(), zonePtr, ZoneConstants::OFFSET_BLOCK_BIT_COUNT, ZoneConstants::INSERT_BLOCK));

    return zoneLoader;
}
