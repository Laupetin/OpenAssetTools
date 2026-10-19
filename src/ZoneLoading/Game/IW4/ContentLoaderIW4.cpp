#include "ContentLoaderIW4.h"

#include "Game/IW4/AssetLoader_iw4_pc32.h"
#include "Game/IW4/AssetLoader_iw4_pc64.h"
#include "Game/IW4/IW4.h"
#include "Loading/Exception/UnsupportedAssetTypeException.h"

#include <cassert>

using namespace IW4;

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
        if (m_word_size == GameWordSize::ARCH_64)
            varScriptStringList->strings = m_stream.Alloc<const char*>(4);
        else
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
#define FALLTHROUGH_ASSET(assetName) case Asset##assetName## ::EnumEntry:
#define LOAD_ASSET_PER_VARIANT(assetName, headerEntry32, headerEntry64)                                                                                        \
    case Asset##assetName## ::EnumEntry:                                                                                                                       \
    {                                                                                                                                                          \
        if (m_word_size == GameWordSize::ARCH_64)                                                                                                              \
        {                                                                                                                                                      \
            Loader_##assetName##_pc64 loader(m_zone, m_stream);                                                                                                \
            loader.Load(&varXAsset->header.headerEntry64);                                                                                                     \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            Loader_##assetName##_pc32 loader(m_zone, m_stream);                                                                                                \
            loader.Load(&varXAsset->header.headerEntry32);                                                                                                     \
        }                                                                                                                                                      \
        break;                                                                                                                                                 \
    }
#define LOAD_ASSET(assetName, headerEntry) LOAD_ASSET_PER_VARIANT(assetName, headerEntry, headerEntry)

    assert(varXAsset != nullptr);

    if (atStreamStart)
        m_stream.Load<XAsset>(varXAsset);

    switch (varXAsset->type)
    {
        LOAD_ASSET(PhysPreset, physPreset)
        LOAD_ASSET(PhysCollMap, physCollmap)
        LOAD_ASSET(XAnim, parts)
        LOAD_ASSET(XModel, model)
        LOAD_ASSET(Material, material)
        LOAD_ASSET(PixelShader, pixelShader)
        LOAD_ASSET(VertexShader, vertexShader)
        LOAD_ASSET(VertexDecl, vertexDecl)
        LOAD_ASSET(TechniqueSet, techniqueSet)
        LOAD_ASSET(Image, image)
        LOAD_ASSET(Sound, sound)
        LOAD_ASSET(SoundCurve, sndCurve)
        LOAD_ASSET_PER_VARIANT(LoadedSound, loadSnd32, loadSnd64)
        FALLTHROUGH_ASSET(ClipMapSp)
        LOAD_ASSET(ClipMapMp, clipMap)
        LOAD_ASSET(ComWorld, comWorld)
        LOAD_ASSET(GameWorldSp, gameWorldSp)
        LOAD_ASSET(GameWorldMp, gameWorldMp)
        LOAD_ASSET(MapEnts, mapEnts)
        LOAD_ASSET(FxWorld, fxWorld)
        LOAD_ASSET(GfxWorld, gfxWorld)
        LOAD_ASSET(LightDef, lightDef)
        LOAD_ASSET(Font, font)
        LOAD_ASSET(MenuList, menuList)
        LOAD_ASSET(Menu, menu)
        LOAD_ASSET(Localize, localize)
        LOAD_ASSET(Weapon, weapon)
        LOAD_ASSET(Fx, fx)
        LOAD_ASSET(ImpactFx, impactFx)
        LOAD_ASSET(RawFile, rawfile)
        LOAD_ASSET(StringTable, stringTable)
        LOAD_ASSET(Leaderboard, leaderboardDef)
        LOAD_ASSET(StructuredDataDef, structuredDataDefSet)
        LOAD_ASSET(Tracer, tracerDef)
        LOAD_ASSET(Vehicle, vehDef)
        LOAD_ASSET(AddonMapEnts, addonMapEnts)

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
        if (m_word_size == GameWordSize::ARCH_64)
        {
            m_stream.Load<XAsset>(varXAsset, count);
        }
        else
        {
            const auto fill = m_stream.LoadWithFill(8u * count);

            for (size_t index = 0; index < count; index++)
            {
                fill.Fill(varXAsset[index].type, 8u * index);
                fill.FillPtr(varXAsset[index].header.data, 8u * index + 4u);
                m_stream.AddPointerLookup(&varXAsset[index].header.data, fill.BlockBuffer(8u * index + 4u));
            }
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
    if (m_word_size == GameWordSize::ARCH_64)
    {
        m_stream.LoadDataRaw(&assetList, sizeof(assetList));
    }
    else
    {
        const auto fillAccessor = m_stream.LoadWithFill(16u);
        varScriptStringList = &varXAssetList->stringList;
        fillAccessor.Fill(varScriptStringList->count, 0u);
        fillAccessor.FillPtr(varScriptStringList->strings, 4u);

        fillAccessor.Fill(varXAssetList->assetCount, 8u);
        fillAccessor.FillPtr(varXAssetList->assets, 12u);
    }
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
        if (m_word_size == GameWordSize::ARCH_64)
            assetList.assets = m_stream.Alloc<XAsset>(4);
        else
            assetList.assets = m_stream.AllocOutOfBlock<XAsset>(4, assetList.assetCount);
#endif
        varXAsset = assetList.assets;
        LoadXAssetArray(true, assetList.assetCount);
    }

    m_stream.PopBlock();
}
