#include "ObjWriterT5.h"

#include "Game/T5/GameAssetPoolT5.h"
#include "Game/T5/Material/MaterialJsonDumperT5.h"
#include "Game/T5/XModel/XModelDumperT5.h"
#include "Image/ImageDumperT5.h"
#include "Localize/LocalizeDumperT5.h"
#include "ObjWriting.h"
#include "PhysConstraints/AssetDumperPhysConstraints.h"
#include "PhysPreset/AssetDumperPhysPreset.h"
#include "RawFile/AssetDumperRawFile.h"
#include "Sound/AssetDumperSndBank.h"
#include "StringTable/AssetDumperStringTable.h"
#include "Weapon/AssetDumperWeapon.h"

using namespace T5;

bool ObjWriter::DumpZone(AssetDumpingContext& context) const
{
#define DUMP_ASSET_POOL(dumperType, poolName, assetType)                                                                                                       \
    if (assetPools->poolName && ObjWriting::ShouldHandleAssetType(assetType))                                                                                  \
    {                                                                                                                                                          \
        dumperType dumper;                                                                                                                                     \
        dumper.DumpPool(context, assetPools->poolName.get());                                                                                                  \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolT5*>(context.m_zone.m_pools.get());

    // DUMP_ASSET_POOL(AssetDumperPhysPreset, m_phys_preset, ASSET_TYPE_PHYSPRESET)
    // DUMP_ASSET_POOL(AssetDumperPhysConstraints, m_phys_constraints, ASSET_TYPE_PHYSCONSTRAINTS)
    // DUMP_ASSET_POOL(AssetDumperDestructibleDef, m_destructible_def, ASSET_TYPE_DESTRUCTIBLEDEF)
    // DUMP_ASSET_POOL(AssetDumperXAnimParts, m_xanim_parts, ASSET_TYPE_XANIMPARTS)
    DUMP_ASSET_POOL(AssetDumperXModel, m_xmodel, ASSET_TYPE_XMODEL)
    DUMP_ASSET_POOL(material::JsonDumper, m_material, ASSET_TYPE_MATERIAL)
    // DUMP_ASSET_POOL(AssetDumperTechniqueSet, m_technique_set, ASSET_TYPE_TECHNIQUE_SET)
    DUMP_ASSET_POOL(image::Dumper, m_image, ASSET_TYPE_IMAGE)
    // DUMP_ASSET_POOL(AssetDumperSndBank, m_sound_bank, ASSET_TYPE_SOUND)
    // DUMP_ASSET_POOL(AssetDumperSndPatch, m_sound_patch, ASSET_TYPE_SOUND_PATCH)
    // DUMP_ASSET_POOL(AssetDumperClipMap, m_clip_map, ASSET_TYPE_CLIPMAP_PVS)
    // DUMP_ASSET_POOL(AssetDumperComWorld, m_com_world, ASSET_TYPE_COMWORLD)
    // DUMP_ASSET_POOL(AssetDumperGameWorldSp, m_game_world_sp, ASSET_TYPE_GAMEWORLD_SP)
    // DUMP_ASSET_POOL(AssetDumperGameWorldMp, m_game_world_mp, ASSET_TYPE_GAMEWORLD_MP)
    // DUMP_ASSET_POOL(AssetDumperMapEnts, m_map_ents, ASSET_TYPE_MAP_ENTS)
    // DUMP_ASSET_POOL(AssetDumperGfxWorld, m_gfx_world, ASSET_TYPE_GFXWORLD)
    // DUMP_ASSET_POOL(AssetDumperGfxLightDef, m_gfx_light_def, ASSET_TYPE_LIGHT_DEF)
    // DUMP_ASSET_POOL(AssetDumperFont, m_font, ASSET_TYPE_FONT)
    // DUMP_ASSET_POOL(AssetDumperMenuList, m_menu_list, ASSET_TYPE_MENULIST)
    // DUMP_ASSET_POOL(AssetDumperMenuDef, m_menu_def, ASSET_TYPE_MENU)
    DUMP_ASSET_POOL(localize::Dumper, m_localize, ASSET_TYPE_LOCALIZE_ENTRY)
    // DUMP_ASSET_POOL(AssetDumperWeapon, m_weapon, ASSET_TYPE_WEAPON)
    // DUMP_ASSET_POOL(AssetDumperSndDriverGlobals, m_snd_driver_globals, ASSET_TYPE_SNDDRIVER_GLOBALS)
    // DUMP_ASSET_POOL(AssetDumperFxEffectDef, m_fx, ASSET_TYPE_FX)
    // DUMP_ASSET_POOL(AssetDumperFxImpactTable, m_fx_impact_table, ASSET_TYPE_IMPACT_FX)
    DUMP_ASSET_POOL(AssetDumperRawFile, m_raw_file, ASSET_TYPE_RAWFILE)
    DUMP_ASSET_POOL(AssetDumperStringTable, m_string_table, ASSET_TYPE_STRINGTABLE)
    // DUMP_ASSET_POOL(AssetDumperPackIndex, m_pack_index, ASSET_TYPE_PACK_INDEX)
    // DUMP_ASSET_POOL(AssetDumperXGlobals, m_xglobals, ASSET_TYPE_XGLOBALS)
    // DUMP_ASSET_POOL(AssetDumperDDLRoot, m_ddl, ASSET_TYPE_DDL)
    // DUMP_ASSET_POOL(AssetDumperGlasses, m_glasses, ASSET_TYPE_GLASSES)
    // DUMP_ASSET_POOL(AssetDumperEmblemSet, m_emblem_set, ASSET_TYPE_EMBLEMSET)

    return true;

#undef DUMP_ASSET_POOL
}
