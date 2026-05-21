#include "ContentLoaderIW6.h"

#include "Game/IW6/AssetLoaderIW6.h"
#include "Game/IW6/IW6.h"
#include "Loading/Exception/UnsupportedAssetTypeException.h"

#include <cassert>

using namespace IW6;

ContentLoader::ContentLoader(Zone& zone, ZoneInputStream& stream)
    : ContentLoaderBase(zone, stream),
      varXAssetList(nullptr),
      varXAsset(nullptr),
      varScriptStringList(nullptr)
{
    // -1
    m_zone_ptr_following =
        reinterpret_cast<const void*>(std::numeric_limits<std::uintptr_t>::max() >> ((sizeof(std::uintptr_t) * 8u) - stream.GetPointerBitCount()));

    // -2
    m_zone_ptr_insert = reinterpret_cast<const void*>(-144680337052532737);
}

void ContentLoader::LoadScriptStringList(const bool atStreamStart)
{
    assert(!atStreamStart);

    if (varScriptStringList->strings != nullptr)
    {
        // assert(GetZonePointerType(varScriptStringList->strings) == ZonePointerType::FOLLOWING);

#ifdef ARCH_x86
        varScriptStringList->strings = m_stream.Alloc<const char*>(4);
#else
        varScriptStringList->strings = m_stream.AllocOutOfBlock<const char*>(8, varScriptStringList->count);
#endif
        varXString = varScriptStringList->strings;
        LoadXStringArray(true, varScriptStringList->count, 8);

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
#define SKIP_ASSET(type_index)                                                                                                                                 \
    case type_index:                                                                                                                                           \
        break;

    assert(varXAsset != nullptr);

    if (atStreamStart)
        m_stream.Load<XAsset>(varXAsset);

    switch (varXAsset->type)
    {
        LOAD_ASSET(ASSET_TYPE_PHYSPRESET, PhysPreset, physPreset)
        LOAD_ASSET(ASSET_TYPE_PHYSCOLLMAP, PhysCollmap, physCollmap)
        LOAD_ASSET(ASSET_TYPE_XANIMPARTS, XAnimParts, parts)
        LOAD_ASSET(ASSET_TYPE_XMODEL_SURFS, XModelSurfs, modelSurfs)
        LOAD_ASSET(ASSET_TYPE_XMODEL, XModel, model)
        LOAD_ASSET(ASSET_TYPE_MATERIAL, Material, material)
        LOAD_ASSET(ASSET_TYPE_COMPUTESHADER, ComputeShader, computeShader)
        LOAD_ASSET(ASSET_TYPE_VERTEXSHADER, MaterialVertexShader, vertexShader)
        LOAD_ASSET(ASSET_TYPE_HULLSHADER, MaterialHullShader, hullShader)
        LOAD_ASSET(ASSET_TYPE_DOMAINSHADER, MaterialDomainShader, domainShader)
        LOAD_ASSET(ASSET_TYPE_PIXELSHADER, MaterialPixelShader, pixelShader)
        LOAD_ASSET(ASSET_TYPE_VERTEXDECL, MaterialVertexDeclaration, vertexDecl)
        LOAD_ASSET(ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet, techniqueSet)
        LOAD_ASSET(ASSET_TYPE_IMAGE, GfxImage, image)
        LOAD_ASSET(ASSET_TYPE_SOUND, snd_alias_list_t, sound)
        LOAD_ASSET(ASSET_TYPE_SOUND_CURVE, SoundCurve, sndCurve)
        LOAD_ASSET(ASSET_TYPE_LPF_CURVE, LpfCurve, lpfCurve)
        LOAD_ASSET(ASSET_TYPE_REVERB_CURVE, ReverbCurve, reverbCurve)
        LOAD_ASSET(ASSET_TYPE_LOADED_SOUND, LoadedSound, loadSnd)
        LOAD_ASSET(ASSET_TYPE_CLIPMAP, clipMap_t, clipMap)
        LOAD_ASSET(ASSET_TYPE_COMWORLD, ComWorld, comWorld)
        LOAD_ASSET(ASSET_TYPE_GLASSWORLD, GlassWorld, glassWorld)
        LOAD_ASSET(ASSET_TYPE_PATHDATA, PathData, pathData)
        LOAD_ASSET(ASSET_TYPE_VEHICLE_TRACK, VehicleTrack, vehicleTrack)
        LOAD_ASSET(ASSET_TYPE_MAP_ENTS, MapEnts, mapEnts)
        LOAD_ASSET(ASSET_TYPE_FXWORLD, FxWorld, fxWorld)
        LOAD_ASSET(ASSET_TYPE_GFXWORLD, GfxWorld, gfxWorld)
        LOAD_ASSET(ASSET_TYPE_LIGHT_DEF, GfxLightDef, lightDef)
        SKIP_ASSET(ASSET_TYPE_UI_MAP)
        LOAD_ASSET(ASSET_TYPE_FONT, Font_s, font)
        LOAD_ASSET(ASSET_TYPE_MENULIST, MenuList, menuList)
        LOAD_ASSET(ASSET_TYPE_MENU, menuDef_t, menu)
        LOAD_ASSET(ASSET_TYPE_ANIMCLASS, AnimationClass, animClass)
        LOAD_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry, localize)
        LOAD_ASSET(ASSET_TYPE_ATTACHMENT, WeaponAttachment, attachment)
        LOAD_ASSET(ASSET_TYPE_WEAPON, WeaponCompleteDef, weapon)
        LOAD_ASSET(ASSET_TYPE_SNDDRIVER_GLOBALS, SndDriverGlobals, sndDriverGlobals)
        LOAD_ASSET(ASSET_TYPE_FX, FxEffectDef, fx)
        LOAD_ASSET(ASSET_TYPE_IMPACT_FX, FxImpactTable, impactFx)
        LOAD_ASSET(ASSET_TYPE_SURFACE_FX, SurfaceFxTable, surfaceFx)
        SKIP_ASSET(ASSET_TYPE_AITYPE)
        SKIP_ASSET(ASSET_TYPE_MPTYPE)
        SKIP_ASSET(ASSET_TYPE_CHARACTER)
        SKIP_ASSET(ASSET_TYPE_XMODELALIAS)
        LOAD_ASSET(ASSET_TYPE_RAWFILE, RawFile, rawfile)
        LOAD_ASSET(ASSET_TYPE_SCRIPTFILE, ScriptFile, scriptfile)
        LOAD_ASSET(ASSET_TYPE_STRINGTABLE, StringTable, stringTable)
        LOAD_ASSET(ASSET_TYPE_LEADERBOARD, LeaderboardDef, leaderboardDef)
        LOAD_ASSET(ASSET_TYPE_STRUCTURED_DATA_DEF, StructuredDataDefSet, structuredDataDefSet)
        LOAD_ASSET(ASSET_TYPE_TRACER, TracerDef, tracerDef)
        LOAD_ASSET(ASSET_TYPE_VEHICLE, VehicleDef, vehDef)
        LOAD_ASSET(ASSET_TYPE_ADDON_MAP_ENTS, AddonMapEnts, addonMapEnts)
        LOAD_ASSET(ASSET_TYPE_NET_CONST_STRINGS, NetConstStrings, netConstStrings)
        LOAD_ASSET(ASSET_TYPE_REVERB_PRESET, ReverbPreset, reverbPreset)
        LOAD_ASSET(ASSET_TYPE_LUA_FILE, LuaFile, luaFile)
        LOAD_ASSET(ASSET_TYPE_SCRIPTABLE, ScriptableDef, scriptable)
        LOAD_ASSET(ASSET_TYPE_COLORIZATION, Colorization, colorization)
        LOAD_ASSET(ASSET_TYPE_COLORIZATIONSET, ColorizationSet, colorizationSet)
        LOAD_ASSET(ASSET_TYPE_TONEMAPPING, ToneMapping, toneMapping)
        LOAD_ASSET(ASSET_TYPE_EQUIPMENT_SND_TABLE, EquipmentSoundTable, equipSndTable)
        LOAD_ASSET(ASSET_TYPE_VECTORFIELD, VectorField, vectorField)
        LOAD_ASSET(ASSET_TYPE_DOPPLER_PRESET, DopplerPreset, dopplerPreset)
        LOAD_ASSET(ASSET_TYPE_PARTICLE_SIM_ANIMATION, FxParticleSimAnimation, particleSimAnimation)

    default:
    {
        throw UnsupportedAssetTypeException(varXAsset->type);
    }
    }

#undef LOAD_ASSET
}

void ContentLoader::LoadXAssetArray(const bool atStreamStart, const size_t count)
{
    assert(count == 0 || varXAsset != nullptr);

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
    const auto fillAccessor = m_stream.LoadWithFill(32u);
    varScriptStringList = &varXAssetList->stringList;
    fillAccessor.Fill(varScriptStringList->count, 0u);
    fillAccessor.FillPtr(varScriptStringList->strings, 8u);

    fillAccessor.Fill(varXAssetList->assetCount, 16u);
    fillAccessor.FillPtr(varXAssetList->assets, 24u);
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
