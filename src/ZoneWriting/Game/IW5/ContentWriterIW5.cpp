#include "ContentWriterIW5.h"

#include "Game/IW5/AssetWriterIW5.h"
#include "Writing/WritingException.h"

#include <cassert>
#include <format>

using namespace IW5;

ContentWriter::ContentWriter(const Zone& zone)
    : ContentWriterBase(zone),
      varXAssetList(nullptr),
      varXAsset(nullptr),
      varScriptStringList(nullptr)
{
}

void ContentWriter::CreateXAssetList(XAssetList& xAssetList, MemoryManager& memory) const
{
    if (!m_zone.m_script_strings.Empty())
    {
        assert(m_zone.m_script_strings.Count() <= SCR_STRING_MAX + 1);
        xAssetList.stringList.count = static_cast<int>(m_zone.m_script_strings.Count());
        xAssetList.stringList.strings = memory.Alloc<const char*>(m_zone.m_script_strings.Count());

        for (auto i = 0u; i < m_zone.m_script_strings.Count(); i++)
            xAssetList.stringList.strings[i] = m_zone.m_script_strings.CValue(i);
    }
    else
    {
        xAssetList.stringList.count = 0;
        xAssetList.stringList.strings = nullptr;
    }

    const auto assetCount = m_zone.m_pools->GetTotalAssetCount();
    if (assetCount > 0)
    {
        xAssetList.assetCount = static_cast<int>(assetCount);
        xAssetList.assets = memory.Alloc<XAsset>(assetCount);

        const auto end = m_zone.m_pools->end();
        auto index = 0u;
        for (auto i = m_zone.m_pools->begin(); i != end; ++i)
        {
            auto& asset = xAssetList.assets[index++];
            asset.type = static_cast<XAssetType>((*i)->m_type);
            asset.header.data = (*i)->m_ptr;
        }
    }
    else
    {
        xAssetList.assetCount = 0;
        xAssetList.assets = nullptr;
    }
}

void ContentWriter::WriteScriptStringList(const bool atStreamStart)
{
    m_stream->PushBlock(XFILE_BLOCK_VIRTUAL);

    if (atStreamStart)
        varScriptStringListWritten = m_stream->Write(varScriptStringList);

    if (varScriptStringList->strings != nullptr)
    {
        m_stream->Align(4);
        varXString = varScriptStringList->strings;
        WriteXStringArray(true, varScriptStringList->count);

#ifdef ARCH_x86
        static_assert(offsetof(ScriptStringList, strings) == 4u);
#endif
        m_stream->MarkFollowing(varScriptStringListWritten.WithInnerOffset(4));
    }

    m_stream->PopBlock();
}

void ContentWriter::WriteXAsset(const bool atStreamStart)
{
#ifdef ARCH_x86
    static_assert(offsetof(XAsset, header.data) == 4u);
#endif
#define WRITE_ASSET(type_index, typeName, headerEntry)                                                                                                         \
    case type_index:                                                                                                                                           \
    {                                                                                                                                                          \
        Writer_##typeName writer(varXAsset->header.headerEntry, m_zone, *m_stream);                                                                            \
        writer.Write(varXAsset->header.headerEntry, varXAssetWritten.WithInnerOffset(4));                                                                      \
        break;                                                                                                                                                 \
    }
#define SKIP_ASSET(type_index, typeName, headerEntry)                                                                                                          \
    case type_index:                                                                                                                                           \
        break;

    assert(varXAsset != nullptr);

    if (atStreamStart)
        varXAssetWritten = m_stream->Write(varXAsset);

    switch (varXAsset->type)
    {
        WRITE_ASSET(ASSET_TYPE_PHYSPRESET, PhysPreset, physPreset)
        WRITE_ASSET(ASSET_TYPE_PHYSCOLLMAP, PhysCollmap, physCollmap)
        WRITE_ASSET(ASSET_TYPE_XANIMPARTS, XAnimParts, parts)
        WRITE_ASSET(ASSET_TYPE_XMODEL_SURFS, XModelSurfs, modelSurfs)
        WRITE_ASSET(ASSET_TYPE_XMODEL, XModel, model)
        WRITE_ASSET(ASSET_TYPE_MATERIAL, Material, material)
        WRITE_ASSET(ASSET_TYPE_PIXELSHADER, MaterialPixelShader, pixelShader)
        WRITE_ASSET(ASSET_TYPE_VERTEXSHADER, MaterialVertexShader, vertexShader)
        WRITE_ASSET(ASSET_TYPE_VERTEXDECL, MaterialVertexDeclaration, vertexDecl)
        WRITE_ASSET(ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet, techniqueSet)
        WRITE_ASSET(ASSET_TYPE_IMAGE, GfxImage, image)
        WRITE_ASSET(ASSET_TYPE_SOUND, snd_alias_list_t, sound)
        WRITE_ASSET(ASSET_TYPE_SOUND_CURVE, SndCurve, sndCurve)
        WRITE_ASSET(ASSET_TYPE_LOADED_SOUND, LoadedSound, loadSnd)
        WRITE_ASSET(ASSET_TYPE_CLIPMAP, clipMap_t, clipMap)
        WRITE_ASSET(ASSET_TYPE_COMWORLD, ComWorld, comWorld)
        WRITE_ASSET(ASSET_TYPE_GLASSWORLD, GlassWorld, glassWorld)
        WRITE_ASSET(ASSET_TYPE_PATHDATA, PathData, pathData)
        WRITE_ASSET(ASSET_TYPE_VEHICLE_TRACK, VehicleTrack, vehicleTrack)
        WRITE_ASSET(ASSET_TYPE_MAP_ENTS, MapEnts, mapEnts)
        WRITE_ASSET(ASSET_TYPE_FXWORLD, FxWorld, fxWorld)
        WRITE_ASSET(ASSET_TYPE_GFXWORLD, GfxWorld, gfxWorld)
        WRITE_ASSET(ASSET_TYPE_LIGHT_DEF, GfxLightDef, lightDef)
        WRITE_ASSET(ASSET_TYPE_FONT, Font_s, font)
        WRITE_ASSET(ASSET_TYPE_MENULIST, MenuList, menuList)
        WRITE_ASSET(ASSET_TYPE_MENU, menuDef_t, menu)
        WRITE_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry, localize)
        WRITE_ASSET(ASSET_TYPE_ATTACHMENT, WeaponAttachment, attachment)
        WRITE_ASSET(ASSET_TYPE_WEAPON, WeaponCompleteDef, weapon)
        WRITE_ASSET(ASSET_TYPE_FX, FxEffectDef, fx)
        WRITE_ASSET(ASSET_TYPE_IMPACT_FX, FxImpactTable, impactFx)
        WRITE_ASSET(ASSET_TYPE_SURFACE_FX, SurfaceFxTable, surfaceFx)
        WRITE_ASSET(ASSET_TYPE_RAWFILE, RawFile, rawfile)
        WRITE_ASSET(ASSET_TYPE_SCRIPTFILE, ScriptFile, scriptfile)
        WRITE_ASSET(ASSET_TYPE_STRINGTABLE, StringTable, stringTable)
        WRITE_ASSET(ASSET_TYPE_LEADERBOARD, LeaderboardDef, leaderboardDef)
        WRITE_ASSET(ASSET_TYPE_STRUCTURED_DATA_DEF, StructuredDataDefSet, structuredDataDefSet)
        WRITE_ASSET(ASSET_TYPE_TRACER, TracerDef, tracerDef)
        WRITE_ASSET(ASSET_TYPE_VEHICLE, VehicleDef, vehDef)
        WRITE_ASSET(ASSET_TYPE_ADDON_MAP_ENTS, AddonMapEnts, addonMapEnts)

    default:
    {
        throw WritingException(std::format("Unsupported asset type: {}.", static_cast<unsigned>(varXAsset->type)));
    }
    }

#undef WRITE_ASSET
#undef SKIP_ASSET
}

void ContentWriter::WriteXAssetArray(const bool atStreamStart, const size_t count)
{
    assert(varXAsset != nullptr);

    if (atStreamStart)
        varXAssetWritten = m_stream->Write(varXAsset, count);

    for (size_t index = 0; index < count; index++)
    {
        WriteXAsset(false);
        varXAsset++;

#ifdef ARCH_x86
        static_assert(sizeof(XAsset) == 8u);
#endif
        varXAssetWritten.Inc(8u);
    }
}

void ContentWriter::WriteContent(ZoneOutputStream& stream)
{
    m_stream = &stream;

    m_stream->PushBlock(XFILE_BLOCK_VIRTUAL);

    MemoryManager memory;
    XAssetList assetList{};

    CreateXAssetList(assetList, memory);

    varXAssetList = &assetList;
    varXAssetListWritten = m_stream->WriteDataRaw(&assetList, sizeof(assetList));

#ifdef ARCH_x86
    static_assert(offsetof(XAssetList, stringList) == 0u);
#endif
    varScriptStringList = &varXAssetList->stringList;
    varScriptStringListWritten = varXAssetListWritten.WithInnerOffset(0);
    WriteScriptStringList(false);

    if (varXAssetList->assets != nullptr)
    {
        m_stream->Align(4);
        varXAsset = varXAssetList->assets;
        WriteXAssetArray(true, varXAssetList->assetCount);

#ifdef ARCH_x86
        static_assert(offsetof(XAssetList, assets) == 12u);
#endif
        m_stream->MarkFollowing(varXAssetListWritten.WithInnerOffset(12));
    }

    m_stream->PopBlock();
}
