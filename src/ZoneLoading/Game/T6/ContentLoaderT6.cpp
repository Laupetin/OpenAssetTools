#include "ContentLoaderT6.h"
#include "Game/T6/T6.h"
#include "Loading/Exception/UnsupportedAssetTypeException.h"

#include <cassert>

#include "XAssets/gen/addonmapents/addonmapents_load_db.h"
#include "XAssets/gen/clipmap_t/clipmap_t_load_db.h"
#include "XAssets/gen/comworld/comworld_load_db.h"
#include "XAssets/gen/ddlroot_t/ddlroot_t_load_db.h"
#include "XAssets/gen/destructibledef/destructibledef_load_db.h"
#include "XAssets/gen/emblemset/emblemset_load_db.h"
#include "XAssets/gen/font_s/font_s_load_db.h"
#include "XAssets/gen/fonticon/fonticon_load_db.h"
#include "XAssets/gen/footstepfxtabledef/footstepfxtabledef_load_db.h"
#include "XAssets/gen/footsteptabledef/footsteptabledef_load_db.h"
#include "XAssets/gen/fxeffectdef/fxeffectdef_load_db.h"
#include "XAssets/gen/fximpacttable/fximpacttable_load_db.h"
#include "XAssets/gen/gameworldmp/gameworldmp_load_db.h"
#include "XAssets/gen/gameworldsp/gameworldsp_load_db.h"
#include "XAssets/gen/gfximage/gfximage_load_db.h"
#include "XAssets/gen/gfxlightdef/gfxlightdef_load_db.h"
#include "XAssets/gen/gfxworld/gfxworld_load_db.h"
#include "XAssets/gen/glasses/glasses_load_db.h"
#include "XAssets/gen/keyvaluepairs/keyvaluepairs_load_db.h"
#include "XAssets/gen/leaderboarddef/leaderboarddef_load_db.h"
#include "XAssets/gen/localizeentry/localizeentry_load_db.h"
#include "XAssets/gen/mapents/mapents_load_db.h"
#include "XAssets/gen/material/material_load_db.h"
#include "XAssets/gen/materialtechniqueset/materialtechniqueset_load_db.h"
#include "XAssets/gen/memoryblock/memoryblock_load_db.h"
#include "XAssets/gen/menudef_t/menudef_t_load_db.h"
#include "XAssets/gen/menulist/menulist_load_db.h"
#include "XAssets/gen/physconstraints/physconstraints_load_db.h"
#include "XAssets/gen/physpreset/physpreset_load_db.h"
#include "XAssets/gen/qdb/qdb_load_db.h"
#include "XAssets/gen/rawfile/rawfile_load_db.h"
#include "XAssets/gen/scriptparsetree/scriptparsetree_load_db.h"
#include "XAssets/gen/skinnedvertsdef/skinnedvertsdef_load_db.h"
#include "XAssets/gen/slug/slug_load_db.h"
#include "XAssets/gen/sndbank/sndbank_load_db.h"
#include "XAssets/gen/snddriverglobals/snddriverglobals_load_db.h"
#include "XAssets/gen/sndpatch/sndpatch_load_db.h"
#include "XAssets/gen/stringtable/stringtable_load_db.h"
#include "XAssets/gen/tracerdef/tracerdef_load_db.h"
#include "XAssets/gen/vehicledef/vehicledef_load_db.h"
#include "XAssets/gen/weaponattachment/weaponattachment_load_db.h"
#include "XAssets/gen/weaponattachmentunique/weaponattachmentunique_load_db.h"
#include "XAssets/gen/weaponcamo/weaponcamo_load_db.h"
#include "XAssets/gen/weaponvariantdef/weaponvariantdef_load_db.h"
#include "XAssets/gen/xanimparts/xanimparts_load_db.h"
#include "XAssets/gen/xglobals/xglobals_load_db.h"
#include "XAssets/gen/xmodel/xmodel_load_db.h"
#include "XAssets/gen/zbarrierdef/zbarrierdef_load_db.h"

using namespace T6;

ContentLoaderT6::ContentLoaderT6()
{
    // Script String 0 is always empty string
    m_script_strings.emplace_back("");
}

void ContentLoaderT6::LoadScriptStringList(ScriptStringList* scriptStringList)
{
    m_stream->PushBlock(XFILE_BLOCK_VIRTUAL);

    if(scriptStringList->strings != nullptr)
    {
        assert(scriptStringList->strings == PTR_FOLLOWING);

        scriptStringList->strings = m_stream->Alloc<const char*>();
        LoadXStringArray(scriptStringList->strings, scriptStringList->count, true);

        for(int i = 0; i < scriptStringList->count; i++)
        {
            m_script_strings.emplace_back(scriptStringList->strings[i]);
        }
    }

    m_stream->PopBlock();
}

void ContentLoaderT6::LoadXAsset(XAsset* pXAsset, const bool atStreamStart)
{
#define LOAD_ASSET(type_index, typeName, headerEntry) \
    case type_index: \
        { \
            Loader_##typeName loader(this, m_zone, m_stream); \
            loader.LoadPtr_##typeName(&pXAsset->header.headerEntry); \
            break; \
        }

    assert(pXAsset != nullptr);

    if(atStreamStart)
        m_stream->Load<XAsset>();

    switch(pXAsset->type)
    {
        LOAD_ASSET(ASSET_TYPE_PHYSPRESET, PhysPreset, physPreset);
        LOAD_ASSET(ASSET_TYPE_PHYSCONSTRAINTS, PhysConstraints, physConstraints);
        LOAD_ASSET(ASSET_TYPE_DESTRUCTIBLEDEF, DestructibleDef, destructibleDef);
        LOAD_ASSET(ASSET_TYPE_XANIMPARTS, XAnimParts, parts);
        LOAD_ASSET(ASSET_TYPE_XMODEL, XModel, model);
        LOAD_ASSET(ASSET_TYPE_MATERIAL, Material, material);
        LOAD_ASSET(ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet, techniqueSet);
        LOAD_ASSET(ASSET_TYPE_IMAGE, GfxImage, image);
        LOAD_ASSET(ASSET_TYPE_SOUND, SndBank, sound);
        LOAD_ASSET(ASSET_TYPE_SOUND_PATCH, SndPatch, soundPatch);
        LOAD_ASSET(ASSET_TYPE_CLIPMAP, clipMap_t, clipMap);
        LOAD_ASSET(ASSET_TYPE_CLIPMAP_PVS, clipMap_t, clipMap);
        LOAD_ASSET(ASSET_TYPE_COMWORLD, ComWorld, comWorld);
        LOAD_ASSET(ASSET_TYPE_GAMEWORLD_SP, GameWorldSp, gameWorldSp);
        LOAD_ASSET(ASSET_TYPE_GAMEWORLD_MP, GameWorldMp, gameWorldMp);
        LOAD_ASSET(ASSET_TYPE_MAP_ENTS, MapEnts, mapEnts);
        LOAD_ASSET(ASSET_TYPE_GFXWORLD, GfxWorld, gfxWorld);
        LOAD_ASSET(ASSET_TYPE_LIGHT_DEF, GfxLightDef, lightDef);
        LOAD_ASSET(ASSET_TYPE_FONT, Font_s, font);
        LOAD_ASSET(ASSET_TYPE_FONTICON, FontIcon, fontIcon);
        LOAD_ASSET(ASSET_TYPE_MENULIST, MenuList, menuList);
        LOAD_ASSET(ASSET_TYPE_MENU, menuDef_t, menu);
        LOAD_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry, localize);
        LOAD_ASSET(ASSET_TYPE_WEAPON, WeaponVariantDef, weapon);
        LOAD_ASSET(ASSET_TYPE_ATTACHMENT, WeaponAttachment, attachment);
        LOAD_ASSET(ASSET_TYPE_ATTACHMENT_UNIQUE, WeaponAttachmentUnique, attachmentUnique);
        LOAD_ASSET(ASSET_TYPE_WEAPON_CAMO, WeaponCamo, weaponCamo);
        LOAD_ASSET(ASSET_TYPE_SNDDRIVER_GLOBALS, SndDriverGlobals, sndDriverGlobals);
        LOAD_ASSET(ASSET_TYPE_FX, FxEffectDef, fx);
        LOAD_ASSET(ASSET_TYPE_IMPACT_FX, FxImpactTable, impactFx);
        LOAD_ASSET(ASSET_TYPE_RAWFILE, RawFile, rawfile);
        LOAD_ASSET(ASSET_TYPE_STRINGTABLE, StringTable, stringTable);
        LOAD_ASSET(ASSET_TYPE_LEADERBOARD, LeaderboardDef, leaderboardDef);
        LOAD_ASSET(ASSET_TYPE_XGLOBALS, XGlobals, xGlobals);
        LOAD_ASSET(ASSET_TYPE_DDL, ddlRoot_t, ddlRoot);
        LOAD_ASSET(ASSET_TYPE_GLASSES, Glasses, glasses);
        LOAD_ASSET(ASSET_TYPE_EMBLEMSET, EmblemSet, emblemSet);
        LOAD_ASSET(ASSET_TYPE_SCRIPTPARSETREE, ScriptParseTree, scriptParseTree);
        LOAD_ASSET(ASSET_TYPE_KEYVALUEPAIRS, KeyValuePairs, keyValuePairs);
        LOAD_ASSET(ASSET_TYPE_VEHICLEDEF, VehicleDef, vehicleDef);
        LOAD_ASSET(ASSET_TYPE_MEMORYBLOCK, MemoryBlock, memoryBlock);
        LOAD_ASSET(ASSET_TYPE_ADDON_MAP_ENTS, AddonMapEnts, addonMapEnts);
        LOAD_ASSET(ASSET_TYPE_TRACER, TracerDef, tracerDef);
        LOAD_ASSET(ASSET_TYPE_SKINNEDVERTS, SkinnedVertsDef, skinnedVertsDef);
        LOAD_ASSET(ASSET_TYPE_QDB, Qdb, qdb);
        LOAD_ASSET(ASSET_TYPE_SLUG, Slug, slug);
        LOAD_ASSET(ASSET_TYPE_FOOTSTEP_TABLE, FootstepTableDef, footstepTableDef);
        LOAD_ASSET(ASSET_TYPE_FOOTSTEPFX_TABLE, FootstepFXTableDef, footstepFXTableDef);
        LOAD_ASSET(ASSET_TYPE_ZBARRIER, ZBarrierDef, zbarrierDef);

    default:
        {
            throw UnsupportedAssetTypeException(pXAsset->type);
        }
    }

#undef LOAD_ASSET
}

void ContentLoaderT6::LoadXAssetArray(XAsset* pArray, const size_t count, const bool atStreamStart)
{
    assert(pArray != nullptr);

    if(atStreamStart)
        m_stream->Load<XAsset>(count);

    size_t assetCounts[ASSET_TYPE_COUNT]{0};

    for(size_t index = 0; index < count; index++)
    {
        assert(pArray[index].type >= 0 && pArray[index].type < ASSET_TYPE_COUNT);

        if(pArray[index].type >= 0 && pArray[index].type < ASSET_TYPE_COUNT)
        {
            assetCounts[pArray[index].type]++;
        }
    }

    // Special case: CLIPMAP and CLIPMAP_PVS are the same struct and therefore share the same asset pool
    assetCounts[ASSET_TYPE_CLIPMAP_PVS] += assetCounts[ASSET_TYPE_CLIPMAP];
    assetCounts[ASSET_TYPE_CLIPMAP] = 0;

    for(asset_type_t assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
    {
        m_zone->GetPools()->InitPoolStatic(assetType, assetCounts[assetType]);
    }

    for(size_t index = 0; index < count; index++)
    {
        LoadXAsset(&pArray[index], false);
    }
}

void ContentLoaderT6::Load(Zone* zone, IZoneInputStream* stream)
{
    m_zone = zone;
    m_stream = stream;

    m_stream->PushBlock(XFILE_BLOCK_VIRTUAL);

    auto* assetList = m_stream->Alloc<XAssetList>();
    m_stream->Load<XAssetList>();

    LoadScriptStringList(&assetList->stringList);

    if(assetList->depends != nullptr)
    {
        assert(assetList->depends == PTR_FOLLOWING);

        assetList->depends = m_stream->Alloc<const char*>();
        LoadXStringArray(assetList->depends, assetList->dependCount, true);
    }

    if(assetList->assets != nullptr)
    {
        assert(assetList->assets == PTR_FOLLOWING);

        assetList->assets = m_stream->Alloc<XAsset>();
        LoadXAssetArray(assetList->assets, assetList->assetCount, true);
    }

    m_stream->PopBlock();
}

std::string& ContentLoaderT6::GetZoneScriptString(const scr_string_t scrString)
{
    assert(scrString >= 0 && scrString < m_script_strings.size());

    if(scrString < 0 || scrString >= m_script_strings.size())
    {
        return m_script_strings[0];
    }

    return m_script_strings[scrString];
}