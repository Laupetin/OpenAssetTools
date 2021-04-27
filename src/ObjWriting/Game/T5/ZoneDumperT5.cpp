#include "ZoneDumperT5.h"

#include "Game/T5/GameT5.h"
#include "Game/T5/GameAssetPoolT5.h"

#include "AssetDumpers/AssetDumperRawFile.h"
#include "AssetDumpers/AssetDumperStringTable.h"
#include "AssetDumpers/AssetDumperLocalizeEntry.h"
#include "AssetDumpers/AssetDumperGfxImage.h"
#include "AssetDumpers/AssetDumperPhysConstraints.h"
#include "AssetDumpers/AssetDumperPhysPreset.h"
#include "AssetDumpers/AssetDumperSndBank.h"
#include "AssetDumpers/AssetDumperWeapon.h"

using namespace T5;

bool ZoneDumper::CanHandleZone(AssetDumpingContext& context) const
{
    return context.m_zone->m_game == &g_GameT5;
}

bool ZoneDumper::DumpZone(AssetDumpingContext& context) const
{
#define DUMP_ASSET_POOL(dumperType, poolName) \
    if(assetPools->poolName) \
    { \
        dumperType dumper; \
        dumper.DumpPool(context, assetPools->poolName); \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolT5*>(context.m_zone->m_pools.get());

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
    // DUMP_ASSET_POOL(AssetDumperMenuList, m_menu_list);
    // DUMP_ASSET_POOL(AssetDumperMenuDef, m_menu_def);
    DUMP_ASSET_POOL(AssetDumperLocalizeEntry, m_localize);
    // DUMP_ASSET_POOL(AssetDumperWeapon, m_weapon);
    // DUMP_ASSET_POOL(AssetDumperSndDriverGlobals, m_snd_driver_globals);
    // DUMP_ASSET_POOL(AssetDumperFxEffectDef, m_fx);
    // DUMP_ASSET_POOL(AssetDumperFxImpactTable, m_fx_impact_table);
    DUMP_ASSET_POOL(AssetDumperRawFile, m_raw_file);
    DUMP_ASSET_POOL(AssetDumperStringTable, m_string_table);
    // DUMP_ASSET_POOL(AssetDumperPackIndex, m_pack_index);
    // DUMP_ASSET_POOL(AssetDumperXGlobals, m_xglobals);
    // DUMP_ASSET_POOL(AssetDumperDDLRoot, m_ddl);
    // DUMP_ASSET_POOL(AssetDumperGlasses, m_glasses);
    // DUMP_ASSET_POOL(AssetDumperEmblemSet, m_emblem_set);

    return true;

#undef DUMP_ASSET_POOL
}
