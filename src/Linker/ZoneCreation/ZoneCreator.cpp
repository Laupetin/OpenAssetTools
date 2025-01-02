#include "ZoneCreator.h"

#include "Gdt/GdtLookup.h"
#include "IObjCompiler.h"
#include "IObjLoader.h"

#include <cassert>

namespace
{
    std::unique_ptr<Zone> CreateZone(const ZoneCreationContext& context, const GameId gameId)
    {
        return std::make_unique<Zone>(context.m_definition->m_name, 0, IGame::GetGameById(gameId));
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
} // namespace

namespace zone_creator
{
    void InitLookup(const ZoneCreationContext& context, GdtLookup& lookup)
    {
        std::vector<const Gdt*> gdtFiles;
        gdtFiles.reserve(context.m_gdt_files.size());

        for (const auto& gdt : context.m_gdt_files)
        {
            gdtFiles.emplace_back(gdt.get());
        }

        lookup.Initialize(gdtFiles);
    }

    std::unique_ptr<Zone> CreateZoneForDefinition(GameId gameId, ZoneCreationContext& context)
    {
        auto zone = CreateZone(context, gameId);

        IgnoreReferencesFromAssets(context);
        IgnoredAssetLookup ignoredAssetLookup(context.m_ignored_assets);

        GdtLookup lookup;
        InitLookup(context, lookup);

        const auto* objCompiler = IObjCompiler::GetObjCompilerForGame(gameId);
        const auto* objLoader = IObjLoader::GetObjLoaderForGame(gameId);

        AssetCreatorCollection creatorCollection(*zone);
        ZoneDefinitionContext zoneDefinitionContext(*context.m_definition);
        objCompiler->ConfigureCreatorCollection(
            creatorCollection, *zone, zoneDefinitionContext, *context.m_asset_search_path, lookup, context.m_out_dir, context.m_cache_dir);
        objLoader->ConfigureCreatorCollection(creatorCollection, *zone, *context.m_asset_search_path, lookup);

        AssetCreationContext creationContext(zone.get(), &creatorCollection, &ignoredAssetLookup);

        for (const auto& assetEntry : context.m_definition->m_assets)
        {
            const auto* createdAsset = creationContext.LoadDependencyGeneric(assetEntry.m_asset_type, assetEntry.m_asset_name);

            if (!createdAsset)
                return nullptr;

            ++zoneDefinitionContext.m_asset_index_in_definition;
        }

        creatorCollection.FinalizeZone(creationContext);

        return zone;
    }
} // namespace zone_creator
