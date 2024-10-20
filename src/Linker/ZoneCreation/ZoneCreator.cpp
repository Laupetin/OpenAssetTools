#include "ZoneCreator.h"

#include "AssetLoading/AssetLoadingContext.h"
#include "Game/IW3/ZoneCreatorIW3.h"
#include "Game/IW4/ZoneCreatorIW4.h"
#include "Game/IW5/ZoneCreatorIW5.h"
#include "Game/T5/ZoneCreatorT5.h"
#include "Game/T6/ZoneCreatorT6.h"
#include "IObjCompiler.h"
#include "IObjLoader.h"

#include <cassert>

const IZoneCreator* IZoneCreator::GetCreatorForGame(GameId game)
{
    static const IZoneCreator* zoneCreators[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::ZoneCreator(),
        new IW4::ZoneCreator(),
        new IW5::ZoneCreator(),
        new T5::ZoneCreator(),
        new T6::ZoneCreator(),
    };
    static_assert(std::extent_v<decltype(zoneCreators)> == static_cast<unsigned>(GameId::COUNT));

    assert(static_cast<unsigned>(game) < static_cast<unsigned>(GameId::COUNT));
    const auto* result = zoneCreators[static_cast<unsigned>(game)];
    assert(result);

    return result;
}

namespace
{
    std::unique_ptr<Zone> CreateZone(const ZoneCreationContext& context, const GameId gameId)
    {
        return std::make_unique<Zone>(context.m_definition->m_name, 0, IGame::GetGameById(gameId));
    }

    void InitializeAssetPools(Zone& zone, const GameId gameId)
    {
        zone.m_pools = ZoneAssetPools::CreateForGame(gameId, &zone, zone.m_priority);
        const auto assetTypeCount = zone.m_pools->GetAssetTypeCount();
        for (auto assetType = 0; assetType < assetTypeCount; assetType++)
            zone.m_pools->InitPoolDynamic(assetType);
    }

    std::vector<Gdt*> CreateGdtList(const ZoneCreationContext& context)
    {
        std::vector<Gdt*> gdtList;
        gdtList.reserve(context.m_gdt_files.size());
        for (const auto& gdt : context.m_gdt_files)
            gdtList.push_back(gdt.get());

        return gdtList;
    }

    void IgnoreReferencesFromAssets(ZoneCreationContext& context)
    {
        for (const auto& assetEntry : context.m_definition->m_assets)
        {
            if (!assetEntry.m_is_reference)
                continue;

            context.m_ignored_assets.m_entries.emplace_back(assetEntry.m_asset_type, assetEntry.m_asset_name, assetEntry.m_is_reference);
        }
    }

    void ApplyIgnoredAssetsToLoadingContext(const ZoneCreationContext& creationContext, AssetLoadingContext& loadingContext)
    {
        for (const auto& ignoreEntry : creationContext.m_ignored_assets.m_entries)
            loadingContext.m_ignored_asset_map[ignoreEntry.m_name] = ignoreEntry.m_type;
    }
} // namespace

std::unique_ptr<Zone> IZoneCreator::CreateZoneForDefinition(ZoneCreationContext& context) const
{
    const auto gameId = GetGameId();

    auto zone = CreateZone(context, gameId);
    InitializeAssetPools(*zone, gameId);

    AssetLoadingContext assetLoadingContext(*zone, *context.m_asset_search_path, CreateGdtList(context));
    IgnoreReferencesFromAssets(context);
    ApplyIgnoredAssetsToLoadingContext(context, assetLoadingContext);

    HandleMetadata(*zone, context);

    const auto* objCompiler = IObjCompiler::GetObjCompilerForGame(gameId);
    const auto* objLoader = IObjLoader::GetObjLoaderForGame(gameId);
    for (const auto& assetEntry : context.m_definition->m_assets)
    {
        const auto compilerResult = objCompiler->CompileAssetForZone(assetLoadingContext, assetEntry.m_asset_type, assetEntry.m_asset_name);
        if (compilerResult == ObjCompilerResult::FAILURE)
            return nullptr;

        if (compilerResult == ObjCompilerResult::NO_COMPILATION_DONE)
        {
            if (!objLoader->LoadAssetForZone(assetLoadingContext, assetEntry.m_asset_type, assetEntry.m_asset_name))
                return nullptr;
        }
    }

    objLoader->FinalizeAssetsForZone(assetLoadingContext);

    return zone;
}
