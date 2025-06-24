#include "ContentLoaderT5.h"

#include "Game/T5/T5.h"
#include "Game/T5/XAssets/clipmap_t/clipmap_t_load_db.h"
#include "Game/T5/XAssets/comworld/comworld_load_db.h"
#include "Game/T5/XAssets/ddlroot_t/ddlroot_t_load_db.h"
#include "Game/T5/XAssets/destructibledef/destructibledef_load_db.h"
#include "Game/T5/XAssets/emblemset/emblemset_load_db.h"
#include "Game/T5/XAssets/font_s/font_s_load_db.h"
#include "Game/T5/XAssets/fxeffectdef/fxeffectdef_load_db.h"
#include "Game/T5/XAssets/fximpacttable/fximpacttable_load_db.h"
#include "Game/T5/XAssets/gameworldmp/gameworldmp_load_db.h"
#include "Game/T5/XAssets/gameworldsp/gameworldsp_load_db.h"
#include "Game/T5/XAssets/gfximage/gfximage_load_db.h"
#include "Game/T5/XAssets/gfxlightdef/gfxlightdef_load_db.h"
#include "Game/T5/XAssets/gfxworld/gfxworld_load_db.h"
#include "Game/T5/XAssets/glasses/glasses_load_db.h"
#include "Game/T5/XAssets/localizeentry/localizeentry_load_db.h"
#include "Game/T5/XAssets/mapents/mapents_load_db.h"
#include "Game/T5/XAssets/material/material_load_db.h"
#include "Game/T5/XAssets/materialtechniqueset/materialtechniqueset_load_db.h"
#include "Game/T5/XAssets/menudef_t/menudef_t_load_db.h"
#include "Game/T5/XAssets/menulist/menulist_load_db.h"
#include "Game/T5/XAssets/packindex/packindex_load_db.h"
#include "Game/T5/XAssets/physconstraints/physconstraints_load_db.h"
#include "Game/T5/XAssets/physpreset/physpreset_load_db.h"
#include "Game/T5/XAssets/rawfile/rawfile_load_db.h"
#include "Game/T5/XAssets/sndbank/sndbank_load_db.h"
#include "Game/T5/XAssets/snddriverglobals/snddriverglobals_load_db.h"
#include "Game/T5/XAssets/sndpatch/sndpatch_load_db.h"
#include "Game/T5/XAssets/stringtable/stringtable_load_db.h"
#include "Game/T5/XAssets/weaponvariantdef/weaponvariantdef_load_db.h"
#include "Game/T5/XAssets/xanimparts/xanimparts_load_db.h"
#include "Game/T5/XAssets/xglobals/xglobals_load_db.h"
#include "Game/T5/XAssets/xmodel/xmodel_load_db.h"
#include "Loading/Exception/UnsupportedAssetTypeException.h"

#include <cassert>

using namespace T5;

ContentLoader::ContentLoader(Zone& zone, ZoneInputStream& stream)
    : ContentLoaderBase(zone, stream),
      varXAssetList(nullptr),
      varXAsset(nullptr),
      varScriptStringList(nullptr)
{
}

void ContentLoader::LoadScriptStringList(const bool atStreamStart)
{
    assert(!atStreamStart);

    if (varScriptStringList->strings != nullptr)
    {
        assert(GetZonePointerType(varScriptStringList->strings) == ZonePointerType::FOLLOWING);

#ifdef ARCH_x86
        varScriptStringList->strings = m_stream.Alloc<const char*>(4);
#else
        varScriptStringList->strings = m_stream.AllocOutOfBlock<const char*>(4, varScriptStringList->count);
#endif
        varXString = varScriptStringList->strings;
        LoadXStringArray(true, varScriptStringList->count);

        if (varScriptStringList->strings && varScriptStringList->count > 0)
            m_zone.m_script_strings.InitializeForExistingZone(varScriptStringList->strings, static_cast<size_t>(varScriptStringList->count));
    }

    assert(m_zone.m_script_strings.Count() <= SCR_STRING_MAX + 1);
}

void ContentLoader::LoadXAsset(const bool atStreamStart) const
{
#define LOAD_ASSET(type_index, typeName, headerEntry)                                                                                                          \
    case type_index:                                                                                                                                           \
    {                                                                                                                                                          \
        Loader_##typeName loader(m_zone, m_stream);                                                                                                            \
        loader.Load(&varXAsset->header.headerEntry);                                                                                                           \
        break;                                                                                                                                                 \
    }
#define SKIP_ASSET(type_index, typeName, headerEntry)                                                                                                          \
    case type_index:                                                                                                                                           \
        break;

    assert(varXAsset != nullptr);

    if (atStreamStart)
        m_stream.Load<XAsset>(varXAsset);

    switch (varXAsset->type)
    {
        LOAD_ASSET(ASSET_TYPE_PHYSPRESET, PhysPreset, physPreset)
        LOAD_ASSET(ASSET_TYPE_PHYSCONSTRAINTS, PhysConstraints, physConstraints)
        LOAD_ASSET(ASSET_TYPE_DESTRUCTIBLEDEF, DestructibleDef, destructibleDef)
        LOAD_ASSET(ASSET_TYPE_XANIMPARTS, XAnimParts, parts)
        LOAD_ASSET(ASSET_TYPE_XMODEL, XModel, model)
        LOAD_ASSET(ASSET_TYPE_MATERIAL, Material, material)
        LOAD_ASSET(ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet, techniqueSet)
        LOAD_ASSET(ASSET_TYPE_IMAGE, GfxImage, image)
        LOAD_ASSET(ASSET_TYPE_SOUND, SndBank, sound)
        LOAD_ASSET(ASSET_TYPE_SOUND_PATCH, SndPatch, soundPatch)
        LOAD_ASSET(ASSET_TYPE_CLIPMAP, clipMap_t, clipMap)
        LOAD_ASSET(ASSET_TYPE_CLIPMAP_PVS, clipMap_t, clipMap)
        LOAD_ASSET(ASSET_TYPE_COMWORLD, ComWorld, comWorld)
        LOAD_ASSET(ASSET_TYPE_GAMEWORLD_SP, GameWorldSp, gameWorldSp)
        LOAD_ASSET(ASSET_TYPE_GAMEWORLD_MP, GameWorldMp, gameWorldMp)
        LOAD_ASSET(ASSET_TYPE_MAP_ENTS, MapEnts, mapEnts)
        LOAD_ASSET(ASSET_TYPE_GFXWORLD, GfxWorld, gfxWorld)
        LOAD_ASSET(ASSET_TYPE_LIGHT_DEF, GfxLightDef, lightDef)
        LOAD_ASSET(ASSET_TYPE_FONT, Font_s, font)
        LOAD_ASSET(ASSET_TYPE_MENULIST, MenuList, menuList)
        LOAD_ASSET(ASSET_TYPE_MENU, menuDef_t, menu)
        LOAD_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry, localize)
        LOAD_ASSET(ASSET_TYPE_WEAPON, WeaponVariantDef, weapon)
        LOAD_ASSET(ASSET_TYPE_SNDDRIVER_GLOBALS, SndDriverGlobals, sndDriverGlobals)
        LOAD_ASSET(ASSET_TYPE_FX, FxEffectDef, fx)
        LOAD_ASSET(ASSET_TYPE_IMPACT_FX, FxImpactTable, impactFx)
        LOAD_ASSET(ASSET_TYPE_RAWFILE, RawFile, rawfile)
        LOAD_ASSET(ASSET_TYPE_STRINGTABLE, StringTable, stringTable)
        LOAD_ASSET(ASSET_TYPE_PACK_INDEX, PackIndex, packIndex)
        LOAD_ASSET(ASSET_TYPE_XGLOBALS, XGlobals, xGlobals)
        LOAD_ASSET(ASSET_TYPE_DDL, ddlRoot_t, ddlRoot)
        LOAD_ASSET(ASSET_TYPE_GLASSES, Glasses, glasses)
        LOAD_ASSET(ASSET_TYPE_EMBLEMSET, EmblemSet, emblemSet)

    default:
    {
        throw UnsupportedAssetTypeException(varXAsset->type);
    }
    }

#undef LOAD_ASSET
}

void ContentLoader::LoadXAssetArray(const bool atStreamStart, const size_t count)
{
    assert(varXAsset != nullptr);

    if (atStreamStart)
    {
#ifdef ARCH_x86
        m_stream.Load<XAsset>(varXAsset, count);
#else
        const auto fill = m_stream.LoadWithFill(8u * count);

        for (size_t index = 0; index < count; index++)
        {
            fill.Fill(varXAsset[index].type, 8u * index);
            fill.FillPtr(varXAsset[index].header.data, 8u * index + 4u);
            m_stream.AddPointerLookup(&varXAsset[index].header.data, fill.BlockBuffer(8u * index + 4u));
        }
#endif
    }

    for (size_t index = 0; index < count; index++)
    {
        LoadXAsset(false);
        varXAsset++;

#ifdef DEBUG_OFFSETS
        m_stream.DebugOffsets(index);
#endif
    }
}

void ContentLoader::Load()
{
    XAssetList assetList{};
    varXAssetList = &assetList;

#ifdef ARCH_x86
    m_stream.LoadDataRaw(&assetList, sizeof(assetList));
#else
    const auto fillAccessor = m_stream.LoadWithFill(16u);
    varScriptStringList = &varXAssetList->stringList;
    fillAccessor.Fill(varScriptStringList->count, 0u);
    fillAccessor.FillPtr(varScriptStringList->strings, 4u);

    fillAccessor.Fill(varXAssetList->assetCount, 8u);
    fillAccessor.FillPtr(varXAssetList->assets, 12u);
#endif

    m_stream.PushBlock(XFILE_BLOCK_VIRTUAL);

    varScriptStringList = &assetList.stringList;
    LoadScriptStringList(false);

    if (assetList.assets != nullptr)
    {
        assert(GetZonePointerType(assetList.assets) == ZonePointerType::FOLLOWING);

#ifdef ARCH_x86
        assetList.assets = m_stream.Alloc<XAsset>(4);
#else
        assetList.assets = m_stream.AllocOutOfBlock<XAsset>(4, assetList.assetCount);
#endif
        varXAsset = assetList.assets;
        LoadXAssetArray(true, assetList.assetCount);
    }

    m_stream.PopBlock();
}
