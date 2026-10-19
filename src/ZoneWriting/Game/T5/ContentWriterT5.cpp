#include "ContentWriterT5.h"

#include "Game/T5/AssetWriter_t5_pc.h"
#include "Writing/WritingException.h"

#include <cassert>
#include <format>

using namespace T5;

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

    const auto assetCount = m_zone.m_pools.GetTotalAssetCount();
    if (assetCount > 0)
    {
        xAssetList.assetCount = static_cast<int>(assetCount);
        xAssetList.assets = memory.Alloc<XAsset>(assetCount);

        const auto end = m_zone.m_pools.end();
        auto index = 0u;
        for (auto i = m_zone.m_pools.begin(); i != end; ++i)
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
    assert(!atStreamStart);

    if (varScriptStringList->strings != nullptr)
    {
        m_stream->Align(4);
        varXString = varScriptStringList->strings;
        WriteXStringArray(true, varScriptStringList->count);

#ifdef ARCH_x86
        static_assert(offsetof(ScriptStringList, strings) == 4u);
#endif
        m_stream->MarkFollowing(varScriptStringListWritten.AtOffset(4));
    }
}

void ContentWriter::WriteXAsset(const bool atStreamStart)
{
#ifdef ARCH_x86
    static_assert(offsetof(XAsset, header.data) == 4u);
#endif
#define FALLTHROUGH_ASSET(assetName) case Asset##assetName## ::EnumEntry:
#define WRITE_ASSET(assetName, headerEntry)                                                                                                                    \
    case Asset##assetName## ::EnumEntry:                                                                                                                       \
    {                                                                                                                                                          \
        Writer_##assetName##_pc writer(varXAsset->header.headerEntry, m_zone, *m_stream);                                                                      \
        writer.Write(varXAsset->header.headerEntry, varXAssetWritten.AtOffset(4));                                                                             \
        break;                                                                                                                                                 \
    }

    assert(varXAsset != nullptr);

    if (atStreamStart)
        varXAssetWritten = m_stream->Write(varXAsset);

    switch (varXAsset->type)
    {
        WRITE_ASSET(PhysPreset, physPreset)
        WRITE_ASSET(PhysConstraints, physConstraints)
        WRITE_ASSET(DestructibleDef, destructibleDef)
        WRITE_ASSET(XAnim, parts)
        WRITE_ASSET(XModel, model)
        WRITE_ASSET(Material, material)
        WRITE_ASSET(TechniqueSet, techniqueSet)
        WRITE_ASSET(Image, image)
        WRITE_ASSET(SoundBank, sound)
        WRITE_ASSET(SoundPatch, soundPatch)
        FALLTHROUGH_ASSET(ClipMap)
        WRITE_ASSET(ClipMapPvs, clipMap)
        WRITE_ASSET(ComWorld, comWorld)
        WRITE_ASSET(GameWorldSp, gameWorldSp)
        WRITE_ASSET(GameWorldMp, gameWorldMp)
        WRITE_ASSET(MapEnts, mapEnts)
        WRITE_ASSET(GfxWorld, gfxWorld)
        WRITE_ASSET(LightDef, lightDef)
        WRITE_ASSET(Font, font)
        WRITE_ASSET(MenuList, menuList)
        WRITE_ASSET(Menu, menu)
        WRITE_ASSET(Localize, localize)
        WRITE_ASSET(Weapon, weapon)
        WRITE_ASSET(SoundDriverGlobals, sndDriverGlobals)
        WRITE_ASSET(Fx, fx)
        WRITE_ASSET(ImpactFx, impactFx)
        WRITE_ASSET(RawFile, rawfile)
        WRITE_ASSET(StringTable, stringTable)
        WRITE_ASSET(PackIndex, packIndex)
        WRITE_ASSET(XGlobals, xGlobals)
        WRITE_ASSET(DDL, ddlRoot)
        WRITE_ASSET(Glasses, glasses)
        WRITE_ASSET(EmblemSet, emblemSet)

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

#ifdef ARCH_x86
    static_assert(sizeof(XAsset) == 8u);
#endif

    if (atStreamStart)
    {
#ifdef ARCH_x86
        varXAssetWritten = m_stream->Write(varXAsset, count);
#else
        const auto fill = m_stream->WriteWithFill(8u * count);
        varXAssetWritten = fill.Offset();

        for (size_t index = 0; index < count; index++)
            fill.Fill(varXAsset[index].type, 8u * index);
#endif
    }

    for (size_t index = 0; index < count; index++)
    {
        WriteXAsset(false);
        varXAsset++;
        varXAssetWritten.Inc(8u);
    }
}

void ContentWriter::WriteContent(ZoneOutputStream& stream)
{
    m_stream = &stream;

    MemoryManager memory;
    XAssetList assetList{};

    CreateXAssetList(assetList, memory);

    varXAssetList = &assetList;

#ifdef ARCH_x86
    static_assert(sizeof(XAssetList) == 16);
    static_assert(offsetof(XAssetList, assetCount) == 8u);
    varXAssetListWritten = m_stream->WriteDataRaw(&assetList, sizeof(assetList));
#else
    const auto fillAccessor = m_stream->WriteWithFill(16u);
    varXAssetListWritten = fillAccessor.Offset();

    varScriptStringList = &varXAssetList->stringList;
    fillAccessor.Fill(varScriptStringList->count, 0u);

    fillAccessor.Fill(varXAssetList->assetCount, 8u);
#endif

    m_stream->PushBlock(XFILE_BLOCK_VIRTUAL);

#ifdef ARCH_x86
    static_assert(offsetof(XAssetList, stringList) == 0u);
#endif
    varScriptStringList = &varXAssetList->stringList;
    varScriptStringListWritten = varXAssetListWritten.AtOffset(0);
    WriteScriptStringList(false);

    if (varXAssetList->assets != nullptr)
    {
        m_stream->Align(4);
        varXAsset = varXAssetList->assets;
        WriteXAssetArray(true, varXAssetList->assetCount);

#ifdef ARCH_x86
        static_assert(offsetof(XAssetList, assets) == 12u);
#endif
        m_stream->MarkFollowing(varXAssetListWritten.AtOffset(12));
    }

    m_stream->PopBlock();
}
