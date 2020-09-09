#include "ZoneDumperIW4.h"

#include "Game/IW4/GameIW4.h"
#include "Game/IW4/GameAssetPoolIW4.h"

#include "AssetDumpers/AssetDumperRawFile.h"
#include "AssetDumpers/AssetDumperStringTable.h"
#include "AssetDumpers/AssetDumperLocalizeEntry.h"
#include "AssetDumpers/AssetDumperGfxImage.h"

using namespace IW4;

bool ZoneDumper::CanHandleZone(Zone* zone) const
{
    return zone->m_game == &g_GameIW4;
}

bool ZoneDumper::DumpZone(Zone* zone, const std::string& basePath) const
{
#define DUMP_ASSET_POOL(dumperType, poolName) \
    if(assetPools->poolName) \
    { \
        dumperType dumper; \
        dumper.DumpPool(zone, assetPools->poolName.get(), basePath); \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolIW4*>(zone->GetPools());

    // DUMP_ASSET_POOL(AssetDumperPhysPreset, m_phys_preset);
    // DUMP_ASSET_POOL(AssetDumperPhysCollmap, m_phys_collmap);
    // DUMP_ASSET_POOL(AssetDumperXAnimParts, m_xanim_parts);
    // DUMP_ASSET_POOL(AssetDumperXModel, m_xmodel);
    // DUMP_ASSET_POOL(AssetDumperMaterial, m_material);
    // DUMP_ASSET_POOL(AssetDumperMaterialPixelShader, m_material_pixel_shader);
    // DUMP_ASSET_POOL(AssetDumperMaterialVertexShader, m_material_vertex_shader);
    // DUMP_ASSET_POOL(AssetDumperMaterialVertexDeclaration, m_material_vertex_decl);
    // DUMP_ASSET_POOL(AssetDumperMaterialTechniqueSet, m_technique_set);
    // DUMP_ASSET_POOL(AssetDumperGfxImage, m_image);
    // DUMP_ASSET_POOL(AssetDumpersnd_alias_list_t, m_sound);
    // DUMP_ASSET_POOL(AssetDumperSndCurve, m_sound_curve);
    // DUMP_ASSET_POOL(AssetDumperLoadedSound, m_loaded_sound);
    // DUMP_ASSET_POOL(AssetDumperclipMap_t, m_clip_map);
    // DUMP_ASSET_POOL(AssetDumperComWorld, m_com_world);
    // DUMP_ASSET_POOL(AssetDumperGameWorldSp, m_game_world_sp);
    // DUMP_ASSET_POOL(AssetDumperGameWorldMp, m_game_world_mp);
    // DUMP_ASSET_POOL(AssetDumperMapEnts, m_map_ents);
    // DUMP_ASSET_POOL(AssetDumperFxWorld, m_fx_world);
    // DUMP_ASSET_POOL(AssetDumperGfxWorld, m_gfx_world);
    // DUMP_ASSET_POOL(AssetDumperGfxLightDef, m_gfx_light_def);
    // DUMP_ASSET_POOL(AssetDumperFont_s, m_font);
    // DUMP_ASSET_POOL(AssetDumperMenuList, m_menu_list);
    // DUMP_ASSET_POOL(AssetDumpermenuDef_t, m_menu_def);
    DUMP_ASSET_POOL(AssetDumperLocalizeEntry, m_localize);
    // DUMP_ASSET_POOL(AssetDumperWeaponCompleteDef, m_weapon);
    // DUMP_ASSET_POOL(AssetDumperSndDriverGlobals, m_snd_driver_globals);
    // DUMP_ASSET_POOL(AssetDumperFxEffectDef, m_fx);
    // DUMP_ASSET_POOL(AssetDumperFxImpactTable, m_fx_impact_table);
    DUMP_ASSET_POOL(AssetDumperRawFile, m_raw_file);
    DUMP_ASSET_POOL(AssetDumperStringTable, m_string_table);
    // DUMP_ASSET_POOL(AssetDumperLeaderboardDef, m_leaderboard);
    // DUMP_ASSET_POOL(AssetDumperStructuredDataDefSet, m_structed_data_def_set);
    // DUMP_ASSET_POOL(AssetDumperTracerDef, m_tracer);
    // DUMP_ASSET_POOL(AssetDumperVehicleDef, m_vehicle);
    // DUMP_ASSET_POOL(AssetDumperAddonMapEnts, m_addon_map_ents);

    return true;

#undef DUMP_ASSET_POOL
}
