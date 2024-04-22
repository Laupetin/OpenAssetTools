#include "ContentWriterT5.h"

#include "Game/T5/XAssets/clipmap_t/clipmap_t_write_db.h"
#include "Game/T5/XAssets/comworld/comworld_write_db.h"
#include "Game/T5/XAssets/ddlroot_t/ddlroot_t_write_db.h"
#include "Game/T5/XAssets/destructibledef/destructibledef_write_db.h"
#include "Game/T5/XAssets/emblemset/emblemset_write_db.h"
#include "Game/T5/XAssets/font_s/font_s_write_db.h"
#include "Game/T5/XAssets/fxeffectdef/fxeffectdef_write_db.h"
#include "Game/T5/XAssets/fximpacttable/fximpacttable_write_db.h"
#include "Game/T5/XAssets/gameworldmp/gameworldmp_write_db.h"
#include "Game/T5/XAssets/gameworldsp/gameworldsp_write_db.h"
#include "Game/T5/XAssets/gfximage/gfximage_write_db.h"
#include "Game/T5/XAssets/gfxlightdef/gfxlightdef_write_db.h"
#include "Game/T5/XAssets/gfxworld/gfxworld_write_db.h"
#include "Game/T5/XAssets/glasses/glasses_write_db.h"
#include "Game/T5/XAssets/localizeentry/localizeentry_write_db.h"
#include "Game/T5/XAssets/mapents/mapents_write_db.h"
#include "Game/T5/XAssets/material/material_write_db.h"
#include "Game/T5/XAssets/materialtechniqueset/materialtechniqueset_write_db.h"
#include "Game/T5/XAssets/menudef_t/menudef_t_write_db.h"
#include "Game/T5/XAssets/menulist/menulist_write_db.h"
#include "Game/T5/XAssets/packindex/packindex_write_db.h"
#include "Game/T5/XAssets/physconstraints/physconstraints_write_db.h"
#include "Game/T5/XAssets/physpreset/physpreset_write_db.h"
#include "Game/T5/XAssets/rawfile/rawfile_write_db.h"
#include "Game/T5/XAssets/sndbank/sndbank_write_db.h"
#include "Game/T5/XAssets/snddriverglobals/snddriverglobals_write_db.h"
#include "Game/T5/XAssets/sndpatch/sndpatch_write_db.h"
#include "Game/T5/XAssets/stringtable/stringtable_write_db.h"
#include "Game/T5/XAssets/weaponvariantdef/weaponvariantdef_write_db.h"
#include "Game/T5/XAssets/xanimparts/xanimparts_write_db.h"
#include "Game/T5/XAssets/xglobals/xglobals_write_db.h"
#include "Game/T5/XAssets/xmodel/xmodel_write_db.h"
#include "Writing/WritingException.h"

#include <cassert>
#include <sstream>

using namespace T5;

ContentWriter::ContentWriter()
    : varXAssetList(nullptr),
      varXAsset(nullptr),
      varScriptStringList(nullptr)
{
}

void ContentWriter::CreateXAssetList(XAssetList& xAssetList, MemoryManager& memory) const
{
    if (!m_zone->m_script_strings.Empty())
    {
        assert(m_zone->m_script_strings.Count() <= SCR_STRING_MAX + 1);
        xAssetList.stringList.count = static_cast<int>(m_zone->m_script_strings.Count());
        xAssetList.stringList.strings = memory.Alloc<const char*>(m_zone->m_script_strings.Count());

        for (auto i = 0u; i < m_zone->m_script_strings.Count(); i++)
            xAssetList.stringList.strings[i] = m_zone->m_script_strings.CValue(i);
    }
    else
    {
        xAssetList.stringList.count = 0;
        xAssetList.stringList.strings = nullptr;
    }

    const auto assetCount = m_zone->m_pools->GetTotalAssetCount();
    if (assetCount > 0)
    {
        xAssetList.assetCount = static_cast<int>(assetCount);
        xAssetList.assets = memory.Alloc<XAsset>(assetCount);

        const auto end = m_zone->m_pools->end();
        auto index = 0u;
        for (auto i = m_zone->m_pools->begin(); i != end; ++i)
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
        varScriptStringList = m_stream->Write(varScriptStringList);

    if (varScriptStringList->strings != nullptr)
    {
        m_stream->Align(alignof(const char*));
        varXString = varScriptStringList->strings;
        WriteXStringArray(true, varScriptStringList->count);

        m_stream->MarkFollowing(varScriptStringList->strings);
    }

    m_stream->PopBlock();
}

void ContentWriter::WriteXAsset(const bool atStreamStart)
{
#define WRITE_ASSET(type_index, typeName, headerEntry)                                                                                                         \
    case type_index:                                                                                                                                           \
    {                                                                                                                                                          \
        Writer_##typeName writer(varXAsset->header.headerEntry, m_zone, m_stream);                                                                             \
        writer.Write(&varXAsset->header.headerEntry);                                                                                                          \
        break;                                                                                                                                                 \
    }
#define SKIP_ASSET(type_index, typeName, headerEntry)                                                                                                          \
    case type_index:                                                                                                                                           \
        break;

    assert(varXAsset != nullptr);

    if (atStreamStart)
        varXAsset = m_stream->Write(varXAsset);

    switch (varXAsset->type)
    {
        WRITE_ASSET(ASSET_TYPE_PHYSPRESET, PhysPreset, physPreset)
        WRITE_ASSET(ASSET_TYPE_PHYSCONSTRAINTS, PhysConstraints, physConstraints)
        WRITE_ASSET(ASSET_TYPE_DESTRUCTIBLEDEF, DestructibleDef, destructibleDef)
        WRITE_ASSET(ASSET_TYPE_XANIMPARTS, XAnimParts, parts)
        WRITE_ASSET(ASSET_TYPE_XMODEL, XModel, model)
        WRITE_ASSET(ASSET_TYPE_MATERIAL, Material, material)
        WRITE_ASSET(ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet, techniqueSet)
        WRITE_ASSET(ASSET_TYPE_IMAGE, GfxImage, image)
        WRITE_ASSET(ASSET_TYPE_SOUND, SndBank, sound)
        WRITE_ASSET(ASSET_TYPE_SOUND_PATCH, SndPatch, soundPatch)
        WRITE_ASSET(ASSET_TYPE_CLIPMAP, clipMap_t, clipMap)
        WRITE_ASSET(ASSET_TYPE_CLIPMAP_PVS, clipMap_t, clipMap)
        WRITE_ASSET(ASSET_TYPE_COMWORLD, ComWorld, comWorld)
        WRITE_ASSET(ASSET_TYPE_GAMEWORLD_SP, GameWorldSp, gameWorldSp)
        WRITE_ASSET(ASSET_TYPE_GAMEWORLD_MP, GameWorldMp, gameWorldMp)
        WRITE_ASSET(ASSET_TYPE_MAP_ENTS, MapEnts, mapEnts)
        WRITE_ASSET(ASSET_TYPE_GFXWORLD, GfxWorld, gfxWorld)
        WRITE_ASSET(ASSET_TYPE_LIGHT_DEF, GfxLightDef, lightDef)
        WRITE_ASSET(ASSET_TYPE_FONT, Font_s, font)
        WRITE_ASSET(ASSET_TYPE_MENULIST, MenuList, menuList)
        WRITE_ASSET(ASSET_TYPE_MENU, menuDef_t, menu)
        WRITE_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry, localize)
        WRITE_ASSET(ASSET_TYPE_WEAPON, WeaponVariantDef, weapon)
        WRITE_ASSET(ASSET_TYPE_SNDDRIVER_GLOBALS, SndDriverGlobals, sndDriverGlobals)
        WRITE_ASSET(ASSET_TYPE_FX, FxEffectDef, fx)
        WRITE_ASSET(ASSET_TYPE_IMPACT_FX, FxImpactTable, impactFx)
        WRITE_ASSET(ASSET_TYPE_RAWFILE, RawFile, rawfile)
        WRITE_ASSET(ASSET_TYPE_STRINGTABLE, StringTable, stringTable)
        WRITE_ASSET(ASSET_TYPE_PACK_INDEX, PackIndex, packIndex)
        WRITE_ASSET(ASSET_TYPE_XGLOBALS, XGlobals, xGlobals)
        WRITE_ASSET(ASSET_TYPE_DDL, ddlRoot_t, ddlRoot)
        WRITE_ASSET(ASSET_TYPE_GLASSES, Glasses, glasses)
        WRITE_ASSET(ASSET_TYPE_EMBLEMSET, EmblemSet, emblemSet)

    default:
    {
        std::ostringstream str;
        str << "Unsupported asset type: " << varXAsset->type << ".";
        throw WritingException(str.str());
    }
    }

#undef WRITE_ASSET
#undef SKIP_ASSET
}

void ContentWriter::WriteXAssetArray(const bool atStreamStart, const size_t count)
{
    assert(varXAsset != nullptr);

    if (atStreamStart)
        varXAsset = m_stream->Write(varXAsset, count);

    for (size_t index = 0; index < count; index++)
    {
        WriteXAsset(false);
        varXAsset++;
    }
}

void ContentWriter::WriteContent(Zone* zone, IZoneOutputStream* stream)
{
    m_zone = zone;
    m_stream = stream;

    m_stream->PushBlock(XFILE_BLOCK_VIRTUAL);

    MemoryManager memory;
    XAssetList assetList{};

    CreateXAssetList(assetList, memory);

    varXAssetList = static_cast<XAssetList*>(m_stream->WriteDataRaw(&assetList, sizeof(assetList)));

    varScriptStringList = &varXAssetList->stringList;
    WriteScriptStringList(false);

    if (varXAssetList->assets != nullptr)
    {
        m_stream->Align(alignof(XAsset));
        varXAsset = varXAssetList->assets;
        WriteXAssetArray(true, varXAssetList->assetCount);
        m_stream->MarkFollowing(varXAssetList->assets);
    }

    m_stream->PopBlock();
}
