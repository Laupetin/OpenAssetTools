#include "BSPLinker.h"

#include "ClipMapLinker.h"
#include "ComWorldLinker.h"
#include "GameWorldMpLinker.h"
#include "GfxWorldLinker.h"
#include "MapEntsLinker.h"
#include "SkinnedVertsLinker.h"

#include <format>

using namespace T6;

namespace BSP
{
    void BSPLinker::addEmptyFootstepTableAsset(const std::string& assetName) const
    {
        if (assetName.empty())
            return;

        auto* footstepTable = m_memory.Alloc<FootstepTableDef>();
        footstepTable->name = m_memory.Dup(assetName.c_str());
        memset(footstepTable->sndAliasTable, 0, sizeof(footstepTable->sndAliasTable));

        m_context.AddAsset<AssetFootstepTable>(assetName, footstepTable);
    }

    bool BSPLinker::addDefaultRequiredAssets(const BSPData& bsp) const
    {
        if (m_context.LoadDependency<AssetScript>(std::format("maps/mp/{}.gsc", bsp.name)) == nullptr)
            return false;
        if (m_context.LoadDependency<AssetScript>(std::format("maps/mp/{}_amb.gsc", bsp.name)) == nullptr)
            return false;
        if (m_context.LoadDependency<AssetScript>(std::format("maps/mp/{}_fx.gsc", bsp.name)) == nullptr)
            return false;

        if (m_context.LoadDependency<AssetScript>(std::format("clientscripts/mp/{}.csc", bsp.name)) == nullptr)
            return false;
        if (m_context.LoadDependency<AssetScript>(std::format("clientscripts/mp/{}_amb.csc", bsp.name)) == nullptr)
            return false;
        if (m_context.LoadDependency<AssetScript>(std::format("clientscripts/mp/{}_fx.csc", bsp.name)) == nullptr)
            return false;

        addEmptyFootstepTableAsset("default_1st_person");
        addEmptyFootstepTableAsset("default_3rd_person");
        addEmptyFootstepTableAsset("default_1st_person_quiet");
        addEmptyFootstepTableAsset("default_3rd_person_quiet");
        addEmptyFootstepTableAsset("default_3rd_person_loud");
        addEmptyFootstepTableAsset("default_ai");

        if (m_context.LoadDependency<AssetRawFile>("animtrees/fxanim_props.atr") == nullptr)
            return false;

        return true;
    }

    BSPLinker::BSPLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
          m_search_path(searchPath),
          m_context(context)
    {
    }

    bool BSPLinker::linkBSP(const BSPData& bsp) const
    {
        if (!addDefaultRequiredAssets(bsp))
            return false;

        ComWorldLinker comWorldLinker(m_memory, m_search_path, m_context);
        ClipMapLinker clipMapLinker(m_memory, m_search_path, m_context);
        GameWorldMpLinker gameWorldMpLinker(m_memory, m_search_path, m_context);
        GfxWorldLinker gfxWorldLinker(m_memory, m_search_path, m_context);
        MapEntsLinker mapEntsLinker(m_memory, m_search_path, m_context);
        SkinnedVertsLinker skinnedVertsLinker(m_memory, m_search_path, m_context);

        auto* comWorld = comWorldLinker.linkComWorld(bsp);
        if (comWorld == nullptr)
            return false;
        m_context.AddAsset<AssetComWorld>(comWorld->name, comWorld);

        auto* mapEnts = mapEntsLinker.linkMapEnts(bsp);
        if (mapEnts == nullptr)
            return false;
        m_context.AddAsset<AssetMapEnts>(mapEnts->name, mapEnts);

        auto* gameWorldMp = gameWorldMpLinker.linkGameWorldMp(bsp);
        if (gameWorldMp == nullptr)
            return false;
        m_context.AddAsset<AssetGameWorldMp>(gameWorldMp->name, gameWorldMp);

        auto* skinnedVerts = skinnedVertsLinker.linkSkinnedVerts(bsp);
        if (skinnedVerts == nullptr)
            return false;
        m_context.AddAsset<AssetSkinnedVerts>(skinnedVerts->name, skinnedVerts);

        auto* gfxWorld = gfxWorldLinker.linkGfxWorld(bsp); // requires mapents asset
        if (gfxWorld == nullptr)
            return false;
        m_context.AddAsset<AssetGfxWorld>(gfxWorld->name, gfxWorld);

        auto* clipMap = clipMapLinker.linkClipMap(bsp); // requires gfxworld and mapents asset
        if (clipMap == nullptr)
            return false;
        m_context.AddAsset<AssetClipMap>(clipMap->name, clipMap);

        return true;
    }
} // namespace BSP
