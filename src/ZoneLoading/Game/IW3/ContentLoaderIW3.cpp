#include "ContentLoaderIW3.h"

#include "Game/IW3/AssetLoader_iw3_pc.h"
#include "Game/IW3/IW3.h"
#include "Loading/Exception/UnsupportedAssetTypeException.h"

#include <cassert>

using namespace IW3;

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
#define FALLTHROUGH_ASSET(assetName) case Asset##assetName## ::EnumEntry:
#define LOAD_ASSET(assetName, headerEntry)                                                                                                                     \
    case Asset##assetName## ::EnumEntry:                                                                                                                       \
    {                                                                                                                                                          \
        Loader_##assetName##_pc loader(m_zone, m_stream);                                                                                                      \
        loader.Load(&varXAsset->header.headerEntry);                                                                                                           \
        break;                                                                                                                                                 \
    }

    assert(varXAsset != nullptr);

    if (atStreamStart)
        m_stream.Load<XAsset>(varXAsset);

    switch (varXAsset->type)
    {
        LOAD_ASSET(PhysPreset, physPreset)
        LOAD_ASSET(XAnim, parts)
        LOAD_ASSET(XModel, model)
        LOAD_ASSET(Material, material)
        LOAD_ASSET(TechniqueSet, techniqueSet)
        LOAD_ASSET(Image, image)
        LOAD_ASSET(Sound, sound)
        LOAD_ASSET(SoundCurve, sndCurve)
        LOAD_ASSET(LoadedSound, loadSnd)
        FALLTHROUGH_ASSET(ClipMap)
        LOAD_ASSET(ClipMapPvs, clipMap)
        LOAD_ASSET(ComWorld, comWorld)
        LOAD_ASSET(GameWorldSp, gameWorldSp)
        LOAD_ASSET(GameWorldMp, gameWorldMp)
        LOAD_ASSET(MapEnts, mapEnts)
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
