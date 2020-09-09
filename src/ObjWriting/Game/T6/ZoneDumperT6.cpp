#include "ZoneDumperT6.h"

#include "Game/T6/GameT6.h"
#include "Game/T6/GameAssetPoolT6.h"

#include "AssetDumpers/AssetDumperRawFile.h"
#include "AssetDumpers/AssetDumperSlug.h"
#include "AssetDumpers/AssetDumperQdb.h"
#include "AssetDumpers/AssetDumperScriptParseTree.h"
#include "AssetDumpers/AssetDumperStringTable.h"
#include "AssetDumpers/AssetDumperLocalizeEntry.h"
#include "AssetDumpers/AssetDumperGfxImage.h"
#include "AssetDumpers/AssetDumperFontIcon.h"

using namespace T6;

bool ZoneDumper::CanHandleZone(Zone* zone) const
{
    return zone->m_game == &g_GameT6;
}

bool ZoneDumper::DumpZone(Zone* zone, const std::string& basePath) const
{
#define DUMP_ASSET_POOL(dumperType, poolName) \
    if(assetPools->poolName) \
    { \
        dumperType dumper; \
        dumper.DumpPool(zone, assetPools->poolName, basePath); \
    }

    const auto assetPools = dynamic_cast<GameAssetPoolT6*>(zone->GetPools());

    // DUMP_ASSET_POOL(AssetDumperPhysPreset, m_phys_preset);
    // DUMP_ASSET_POOL(AssetDumperPhysConstraints, m_phys_constraints);
    // DUMP_ASSET_POOL(AssetDumperDestructibleDef, m_destructible_def);
    // DUMP_ASSET_POOL(AssetDumperXAnimParts, m_xanim_parts);
    // DUMP_ASSET_POOL(AssetDumperXModel, m_xmodel);
    // DUMP_ASSET_POOL(AssetDumperMaterial, m_material);
    // DUMP_ASSET_POOL(AssetDumperTechniqueSet, m_technique_set);
    DUMP_ASSET_POOL(AssetDumperGfxImage, m_image);
    // DUMP_ASSET_POOL(AssetDumperSndBank, m_sound_bank);
    // DUMP_ASSET_POOL(AssetDumperSndPatch, m_sound_patch);
    // DUMP_ASSET_POOL(AssetDumperClipMap, m_clip_map);
    // DUMP_ASSET_POOL(AssetDumperComWorld, m_com_world);
    // DUMP_ASSET_POOL(AssetDumperGameWorldSp, m_game_world_sp);
    // DUMP_ASSET_POOL(AssetDumperGameWorldMp, m_game_world_mp);
    // DUMP_ASSET_POOL(AssetDumperMapEnts, m_map_ents);
    // DUMP_ASSET_POOL(AssetDumperGfxWorld, m_gfx_world);
    // DUMP_ASSET_POOL(AssetDumperGfxLightDef, m_gfx_light_def);
    // DUMP_ASSET_POOL(AssetDumperFont, m_font);
    DUMP_ASSET_POOL(AssetDumperFontIcon, m_font_icon);
    // DUMP_ASSET_POOL(AssetDumperMenuList, m_menu_list);
    // DUMP_ASSET_POOL(AssetDumperMenuDef, m_menu_def);
    DUMP_ASSET_POOL(AssetDumperLocalizeEntry, m_localize);
    // DUMP_ASSET_POOL(AssetDumperWeaponVariantDef, m_weapon);
    // DUMP_ASSET_POOL(AssetDumperWeaponAttachment, m_attachment);
    // DUMP_ASSET_POOL(AssetDumperWeaponAttachmentUnique, m_attachment_unique);
    // DUMP_ASSET_POOL(AssetDumperWeaponCamo, m_camo);
    // DUMP_ASSET_POOL(AssetDumperSndDriverGlobals, m_snd_driver_globals);
    // DUMP_ASSET_POOL(AssetDumperFxEffectDef, m_fx);
    // DUMP_ASSET_POOL(AssetDumperFxImpactTable, m_fx_impact_table);
    DUMP_ASSET_POOL(AssetDumperRawFile, m_raw_file);
    DUMP_ASSET_POOL(AssetDumperStringTable, m_string_table);
    // DUMP_ASSET_POOL(AssetDumperLeaderboardDef, m_leaderboard);
    // DUMP_ASSET_POOL(AssetDumperXGlobals, m_xglobals);
    // DUMP_ASSET_POOL(AssetDumperDDLRoot, m_ddl);
    // DUMP_ASSET_POOL(AssetDumperGlasses, m_glasses);
    // DUMP_ASSET_POOL(AssetDumperEmblemSet, m_emblem_set);
    DUMP_ASSET_POOL(AssetDumperScriptParseTree, m_script);
    // DUMP_ASSET_POOL(AssetDumperKeyValuePairs, m_key_value_pairs);
    // DUMP_ASSET_POOL(AssetDumperVehicleDef, m_vehicle);
    // DUMP_ASSET_POOL(AssetDumperMemoryBlock, m_memory_block);
    // DUMP_ASSET_POOL(AssetDumperAddonMapEnts, m_addon_map_ents);
    // DUMP_ASSET_POOL(AssetDumperTracerDef, m_tracer);
    // DUMP_ASSET_POOL(AssetDumperSkinnedVertsDef, m_skinned_verts);
    DUMP_ASSET_POOL(AssetDumperQdb, m_qdb);
    DUMP_ASSET_POOL(AssetDumperSlug, m_slug);
    // DUMP_ASSET_POOL(AssetDumperFootstepTableDef, m_footstep_table);
    // DUMP_ASSET_POOL(AssetDumperFootstepFXTableDef, m_footstep_fx_table);
    // DUMP_ASSET_POOL(AssetDumperZBarrierDef, m_zbarrier);

    return true;

#undef DUMP_ASSET_POOL
}
