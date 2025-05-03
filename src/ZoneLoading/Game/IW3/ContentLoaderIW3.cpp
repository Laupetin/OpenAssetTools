#include "ContentLoaderIW3.h"

#include "Game/IW3/IW3.h"
#include "Game/IW3/XAssets/clipmap_t/clipmap_t_load_db.h"
#include "Game/IW3/XAssets/comworld/comworld_load_db.h"
#include "Game/IW3/XAssets/font_s/font_s_load_db.h"
#include "Game/IW3/XAssets/fxeffectdef/fxeffectdef_load_db.h"
#include "Game/IW3/XAssets/fximpacttable/fximpacttable_load_db.h"
#include "Game/IW3/XAssets/gameworldmp/gameworldmp_load_db.h"
#include "Game/IW3/XAssets/gameworldsp/gameworldsp_load_db.h"
#include "Game/IW3/XAssets/gfximage/gfximage_load_db.h"
#include "Game/IW3/XAssets/gfxlightdef/gfxlightdef_load_db.h"
#include "Game/IW3/XAssets/gfxworld/gfxworld_load_db.h"
#include "Game/IW3/XAssets/loadedsound/loadedsound_load_db.h"
#include "Game/IW3/XAssets/localizeentry/localizeentry_load_db.h"
#include "Game/IW3/XAssets/mapents/mapents_load_db.h"
#include "Game/IW3/XAssets/material/material_load_db.h"
#include "Game/IW3/XAssets/materialtechniqueset/materialtechniqueset_load_db.h"
#include "Game/IW3/XAssets/menudef_t/menudef_t_load_db.h"
#include "Game/IW3/XAssets/menulist/menulist_load_db.h"
#include "Game/IW3/XAssets/physpreset/physpreset_load_db.h"
#include "Game/IW3/XAssets/rawfile/rawfile_load_db.h"
#include "Game/IW3/XAssets/snd_alias_list_t/snd_alias_list_t_load_db.h"
#include "Game/IW3/XAssets/sndcurve/sndcurve_load_db.h"
#include "Game/IW3/XAssets/stringtable/stringtable_load_db.h"
#include "Game/IW3/XAssets/weapondef/weapondef_load_db.h"
#include "Game/IW3/XAssets/xanimparts/xanimparts_load_db.h"
#include "Game/IW3/XAssets/xmodel/xmodel_load_db.h"
#include "Loading/Exception/UnsupportedAssetTypeException.h"

#include <cassert>

using namespace IW3;

ContentLoader::ContentLoader(Zone& zone, ZoneInputStream& stream)
    : ContentLoaderBase(zone, stream),
      varXAsset(nullptr),
      varScriptStringList(nullptr)
{
}

void ContentLoader::LoadScriptStringList(const bool atStreamStart)
{
    m_stream.PushBlock(XFILE_BLOCK_VIRTUAL);

    if (atStreamStart)
        m_stream.Load<ScriptStringList>(varScriptStringList);

    if (varScriptStringList->strings != nullptr)
    {
        assert(GetZonePointerType(varScriptStringList->strings) == ZonePointerType::FOLLOWING);

        varScriptStringList->strings = m_stream.Alloc<const char*>(alignof(const char*));
        varXString = varScriptStringList->strings;
        LoadXStringArray(true, varScriptStringList->count);

        if (varScriptStringList->strings && varScriptStringList->count > 0)
            m_zone.m_script_strings.InitializeForExistingZone(varScriptStringList->strings, static_cast<size_t>(varScriptStringList->count));
    }

    m_stream.PopBlock();

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
        LOAD_ASSET(ASSET_TYPE_XANIMPARTS, XAnimParts, parts)
        LOAD_ASSET(ASSET_TYPE_XMODEL, XModel, model)
        LOAD_ASSET(ASSET_TYPE_MATERIAL, Material, material)
        LOAD_ASSET(ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet, techniqueSet)
        LOAD_ASSET(ASSET_TYPE_IMAGE, GfxImage, image)
        LOAD_ASSET(ASSET_TYPE_SOUND, snd_alias_list_t, sound)
        LOAD_ASSET(ASSET_TYPE_SOUND_CURVE, SndCurve, sndCurve)
        LOAD_ASSET(ASSET_TYPE_LOADED_SOUND, LoadedSound, loadSnd)
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
        LOAD_ASSET(ASSET_TYPE_WEAPON, WeaponDef, weapon)
        SKIP_ASSET(ASSET_TYPE_SNDDRIVER_GLOBALS, SndDriverGlobals, sndDriverGlobals)
        LOAD_ASSET(ASSET_TYPE_FX, FxEffectDef, fx)
        LOAD_ASSET(ASSET_TYPE_IMPACT_FX, FxImpactTable, impactFx)
        LOAD_ASSET(ASSET_TYPE_RAWFILE, RawFile, rawfile)
        LOAD_ASSET(ASSET_TYPE_STRINGTABLE, StringTable, stringTable)

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
        m_stream.Load<XAsset>(varXAsset, count);

    for (size_t index = 0; index < count; index++)
    {
        LoadXAsset(false);
        varXAsset++;
    }
}

void ContentLoader::Load()
{
    m_stream.PushBlock(XFILE_BLOCK_VIRTUAL);

    XAssetList assetList{};
    m_stream.LoadDataRaw(&assetList, sizeof(assetList));

    varScriptStringList = &assetList.stringList;
    LoadScriptStringList(false);

    if (assetList.assets != nullptr)
    {
        assert(GetZonePointerType(assetList.assets) == ZonePointerType::FOLLOWING);

        assetList.assets = m_stream.Alloc<XAsset>(alignof(XAsset));
        varXAsset = assetList.assets;
        LoadXAssetArray(true, assetList.assetCount);
    }

    m_stream.PopBlock();
}
