#include "ZoneDumperT6.h"

#include "AssetDumpers/AssetDumperFontIcon.h"
#include "AssetDumpers/AssetDumperGfxImage.h"
#include "AssetDumpers/AssetDumperLocalizeEntry.h"
#include "AssetDumpers/AssetDumperPhysConstraints.h"
#include "AssetDumpers/AssetDumperPhysPreset.h"
#include "AssetDumpers/AssetDumperQdb.h"
#include "AssetDumpers/AssetDumperRawFile.h"
#include "AssetDumpers/AssetDumperScriptParseTree.h"
#include "AssetDumpers/AssetDumperSlug.h"
#include "AssetDumpers/AssetDumperSndBank.h"
#include "AssetDumpers/AssetDumperSndDriverGlobals.h"
#include "AssetDumpers/AssetDumperStringTable.h"
#include "AssetDumpers/AssetDumperTracer.h"
#include "AssetDumpers/AssetDumperVehicle.h"
#include "AssetDumpers/AssetDumperWeapon.h"
#include "AssetDumpers/AssetDumperWeaponAttachment.h"
#include "AssetDumpers/AssetDumperWeaponAttachmentUnique.h"
#include "AssetDumpers/AssetDumperXModel.h"
#include "AssetDumpers/AssetDumperZBarrier.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "Game/T6/GameT6.h"
#include "ObjWriting.h"

using namespace T6;

bool ZoneDumper::CanHandleZone(AssetDumpingContext& context) const
{
    return context.m_zone->m_game == &g_GameT6;
}

bool ZoneDumper::DumpZone(AssetDumpingContext& context) const
{
#define DUMP_ASSET_POOL(dumperType, poolName, assetType)                                                                                                       \
    if (assetPools->poolName && ObjWriting::ShouldHandleAssetType(assetType))                                                                                  \
    {                                                                                                                                                          \
        dumperType dumper;                                                                                                                                     \
        dumper.DumpPool(context, assetPools->poolName.get());                                                                                                  \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolT6*>(context.m_zone->m_pools.get());

    DUMP_ASSET_POOL(AssetDumperPhysPreset, m_phys_preset, ASSET_TYPE_PHYSPRESET)
    DUMP_ASSET_POOL(AssetDumperPhysConstraints, m_phys_constraints, ASSET_TYPE_PHYSCONSTRAINTS)
    // DUMP_ASSET_POOL(AssetDumperDestructibleDef, m_destructible_def, ASSET_TYPE_DESTRUCTIBLEDEF)
    // DUMP_ASSET_POOL(AssetDumperXAnimParts, m_xanim_parts, ASSET_TYPE_XANIMPARTS)
    // DUMP_ASSET_POOL(AssetDumperXModel, m_xmodel, ASSET_TYPE_XMODEL)
    // DUMP_ASSET_POOL(AssetDumperMaterial, m_material, ASSET_TYPE_MATERIAL)
    // DUMP_ASSET_POOL(AssetDumperTechniqueSet, m_technique_set, ASSET_TYPE_TECHNIQUE_SET)
    DUMP_ASSET_POOL(AssetDumperGfxImage, m_image, ASSET_TYPE_IMAGE)
    DUMP_ASSET_POOL(AssetDumperSndBank, m_sound_bank, ASSET_TYPE_SOUND)
    // DUMP_ASSET_POOL(AssetDumperSndPatch, m_sound_patch, ASSET_TYPE_SOUND_PATCH)
    // DUMP_ASSET_POOL(AssetDumperClipMap, m_clip_map, ASSET_TYPE_CLIPMAP)
    // DUMP_ASSET_POOL(AssetDumperComWorld, m_com_world, ASSET_TYPE_COMWORLD)
    // DUMP_ASSET_POOL(AssetDumperGameWorldSp, m_game_world_sp, ASSET_TYPE_GAMEWORLD_SP)
    // DUMP_ASSET_POOL(AssetDumperGameWorldMp, m_game_world_mp, ASSET_TYPE_GAMEWORLD_MP)
    // DUMP_ASSET_POOL(AssetDumperMapEnts, m_map_ents, ASSET_TYPE_MAP_ENTS)
    // DUMP_ASSET_POOL(AssetDumperGfxWorld, m_gfx_world, ASSET_TYPE_GFXWORLD)
    // DUMP_ASSET_POOL(AssetDumperGfxLightDef, m_gfx_light_def, ASSET_TYPE_LIGHT_DEF)
    // DUMP_ASSET_POOL(AssetDumperFont, m_font, ASSET_TYPE_FONT)
    DUMP_ASSET_POOL(AssetDumperFontIcon, m_font_icon, ASSET_TYPE_FONTICON)
    // DUMP_ASSET_POOL(AssetDumperMenuList, m_menu_list, ASSET_TYPE_MENULIST)
    // DUMP_ASSET_POOL(AssetDumperMenuDef, m_menu_def, ASSET_TYPE_MENU)
    DUMP_ASSET_POOL(AssetDumperLocalizeEntry, m_localize, ASSET_TYPE_LOCALIZE_ENTRY)
    DUMP_ASSET_POOL(AssetDumperWeapon, m_weapon, ASSET_TYPE_WEAPON)
    DUMP_ASSET_POOL(AssetDumperWeaponAttachment, m_attachment, ASSET_TYPE_ATTACHMENT)
    DUMP_ASSET_POOL(AssetDumperWeaponAttachmentUnique, m_attachment_unique, ASSET_TYPE_ATTACHMENT_UNIQUE)
    // DUMP_ASSET_POOL(AssetDumperWeaponCamo, m_camo, ASSET_TYPE_WEAPON_CAMO)
    DUMP_ASSET_POOL(AssetDumperSndDriverGlobals, m_snd_driver_globals, ASSET_TYPE_SNDDRIVER_GLOBALS)
    // DUMP_ASSET_POOL(AssetDumperFxEffectDef, m_fx, ASSET_TYPE_FX)
    // DUMP_ASSET_POOL(AssetDumperFxImpactTable, m_fx_impact_table, ASSET_TYPE_IMPACT_FX)
    DUMP_ASSET_POOL(AssetDumperRawFile, m_raw_file, ASSET_TYPE_RAWFILE)
    DUMP_ASSET_POOL(AssetDumperStringTable, m_string_table, ASSET_TYPE_STRINGTABLE)
    // DUMP_ASSET_POOL(AssetDumperLeaderboardDef, m_leaderboard, ASSET_TYPE_LEADERBOARD)
    // DUMP_ASSET_POOL(AssetDumperXGlobals, m_xglobals, ASSET_TYPE_XGLOBALS)
    // DUMP_ASSET_POOL(AssetDumperDDLRoot, m_ddl, ASSET_TYPE_DDL)
    // DUMP_ASSET_POOL(AssetDumperGlasses, m_glasses, ASSET_TYPE_GLASSES)
    // DUMP_ASSET_POOL(AssetDumperEmblemSet, m_emblem_set, ASSET_TYPE_EMBLEMSET)
    DUMP_ASSET_POOL(AssetDumperScriptParseTree, m_script, ASSET_TYPE_SCRIPTPARSETREE)
    // DUMP_ASSET_POOL(AssetDumperKeyValuePairs, m_key_value_pairs, ASSET_TYPE_KEYVALUEPAIRS)
    DUMP_ASSET_POOL(AssetDumperVehicle, m_vehicle, ASSET_TYPE_VEHICLEDEF)
    // DUMP_ASSET_POOL(AssetDumperMemoryBlock, m_memory_block, ASSET_TYPE_MEMORYBLOCK)
    // DUMP_ASSET_POOL(AssetDumperAddonMapEnts, m_addon_map_ents, ASSET_TYPE_ADDON_MAP_ENTS)
    DUMP_ASSET_POOL(AssetDumperTracer, m_tracer, ASSET_TYPE_TRACER)
    // DUMP_ASSET_POOL(AssetDumperSkinnedVertsDef, m_skinned_verts, ASSET_TYPE_SKINNEDVERTS)
    DUMP_ASSET_POOL(AssetDumperQdb, m_qdb, ASSET_TYPE_QDB)
    DUMP_ASSET_POOL(AssetDumperSlug, m_slug, ASSET_TYPE_SLUG)
    // DUMP_ASSET_POOL(AssetDumperFootstepTableDef, m_footstep_table, ASSET_TYPE_FOOTSTEP_TABLE)
    // DUMP_ASSET_POOL(AssetDumperFootstepFXTableDef, m_footstep_fx_table, ASSET_TYPE_FOOTSTEPFX_TABLE)
    DUMP_ASSET_POOL(AssetDumperZBarrier, m_zbarrier, ASSET_TYPE_ZBARRIER)

    return true;

#undef DUMP_ASSET_POOL
}
