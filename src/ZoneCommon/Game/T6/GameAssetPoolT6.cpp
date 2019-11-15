#include "GameAssetPoolT6.h"
#include "Pool/AssetPoolStatic.h"
#include "Pool/AssetPoolDynamic.h"
#include <cassert>

using namespace T6;

GameAssetPoolT6::GameAssetPoolT6(const int priority)
{
    m_priority = priority;

    m_phys_preset = nullptr;
    m_phys_constraints = nullptr;
    m_destructible_def = nullptr;
    m_xanim_parts = nullptr;
    m_xmodel = nullptr;
    m_material = nullptr;
    m_technique_set = nullptr;
    m_image = nullptr;
    m_sound_bank = nullptr;
    m_sound_patch = nullptr;
    m_clip_map = nullptr;
    m_com_world = nullptr;
    m_game_world_sp = nullptr;
    m_game_world_mp = nullptr;
    m_map_ents = nullptr;
    m_gfx_world = nullptr;
    m_gfx_light_def = nullptr;
    m_font = nullptr;
    m_font_icon = nullptr;
    m_menu_list = nullptr;
    m_menu_def = nullptr;
    m_localize = nullptr;
    m_weapon = nullptr;
    m_attachment = nullptr;
    m_attachment_unique = nullptr;
    m_camo = nullptr;
    m_snd_driver_globals = nullptr;
    m_fx = nullptr;
    m_fx_impact_table = nullptr;
    m_raw_file = nullptr;
    m_string_table = nullptr;
    m_leaderboard = nullptr;
    m_xglobals = nullptr;
    m_ddl = nullptr;
    m_glasses = nullptr;
    m_emblem_set = nullptr;
    m_script = nullptr;
    m_key_value_pairs = nullptr;
    m_vehicle = nullptr;
    m_memory_block = nullptr;
    m_addon_map_ents = nullptr;
    m_tracer = nullptr;
    m_skinned_verts = nullptr;
    m_qdb = nullptr;
    m_slug = nullptr;
    m_footstep_table = nullptr;
    m_footstep_fx_table = nullptr;
    m_zbarrier = nullptr;
}

GameAssetPoolT6::~GameAssetPoolT6()
{
#define DELETE_POOL(poolName) \
    delete (poolName); (poolName) = nullptr;

    DELETE_POOL(m_phys_preset);
    DELETE_POOL(m_phys_constraints);
    DELETE_POOL(m_destructible_def);
    DELETE_POOL(m_xanim_parts);
    DELETE_POOL(m_xmodel);
    DELETE_POOL(m_material);
    DELETE_POOL(m_technique_set);
    DELETE_POOL(m_image);
    DELETE_POOL(m_sound_bank);
    DELETE_POOL(m_sound_patch);
    DELETE_POOL(m_clip_map);
    DELETE_POOL(m_com_world);
    DELETE_POOL(m_game_world_sp);
    DELETE_POOL(m_game_world_mp);
    DELETE_POOL(m_map_ents);
    DELETE_POOL(m_gfx_world);
    DELETE_POOL(m_gfx_light_def);
    DELETE_POOL(m_font);
    DELETE_POOL(m_font_icon);
    DELETE_POOL(m_menu_list);
    DELETE_POOL(m_menu_def);
    DELETE_POOL(m_localize);
    DELETE_POOL(m_weapon);
    DELETE_POOL(m_attachment);
    DELETE_POOL(m_attachment_unique);
    DELETE_POOL(m_camo);
    DELETE_POOL(m_snd_driver_globals);
    DELETE_POOL(m_fx);
    DELETE_POOL(m_fx_impact_table);
    DELETE_POOL(m_raw_file);
    DELETE_POOL(m_string_table);
    DELETE_POOL(m_leaderboard);
    DELETE_POOL(m_xglobals);
    DELETE_POOL(m_ddl);
    DELETE_POOL(m_glasses);
    DELETE_POOL(m_emblem_set);
    DELETE_POOL(m_script);
    DELETE_POOL(m_key_value_pairs);
    DELETE_POOL(m_vehicle);
    DELETE_POOL(m_memory_block);
    DELETE_POOL(m_addon_map_ents);
    DELETE_POOL(m_tracer);
    DELETE_POOL(m_skinned_verts);
    DELETE_POOL(m_qdb);
    DELETE_POOL(m_slug);
    DELETE_POOL(m_footstep_table);
    DELETE_POOL(m_footstep_fx_table);
    DELETE_POOL(m_zbarrier);

#undef DELETE_POOL
}

void GameAssetPoolT6::InitPoolStatic(const asset_type_t type, const size_t capacity)
{
#define CASE_INIT_POOL_STATIC(assetType, poolName, poolType) \
    case assetType: \
    { \
        if((poolName) == nullptr && capacity > 0)  \
        { \
            (poolName) = new AssetPoolStatic<poolType>(capacity, m_priority); \
        } \
        break; \
    }

    switch(type)
    {
        CASE_INIT_POOL_STATIC(ASSET_TYPE_PHYSPRESET, m_phys_preset, PhysPreset);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_PHYSCONSTRAINTS, m_phys_constraints, PhysConstraints);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_DESTRUCTIBLEDEF, m_destructible_def, DestructibleDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_XANIMPARTS, m_xanim_parts, XAnimParts);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_XMODEL, m_xmodel, XModel);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MATERIAL, m_material, Material);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_TECHNIQUE_SET, m_technique_set, MaterialTechniqueSet);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_IMAGE, m_image, GfxImage);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SOUND, m_sound_bank, SndBank);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SOUND_PATCH, m_sound_patch, SndPatch);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_CLIPMAP, m_clip_map, clipMap_t);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_CLIPMAP_PVS, m_clip_map, clipMap_t);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_COMWORLD, m_com_world, ComWorld);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_GAMEWORLD_SP, m_game_world_sp, GameWorldSp);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_GAMEWORLD_MP, m_game_world_mp, GameWorldMp);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MAP_ENTS, m_map_ents, MapEnts);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_GFXWORLD, m_gfx_world, GfxWorld);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def, GfxLightDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_FONT, m_font, Font_s);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_FONTICON, m_font_icon, FontIcon);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MENULIST, m_menu_list, MenuList);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MENU, m_menu_def, menuDef_t);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_LOCALIZE_ENTRY, m_localize, LocalizeEntry);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_WEAPON, m_weapon, WeaponVariantDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_ATTACHMENT, m_attachment, WeaponAttachment);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_ATTACHMENT_UNIQUE, m_attachment_unique, WeaponAttachmentUnique);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_WEAPON_CAMO, m_camo, WeaponCamo);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SNDDRIVER_GLOBALS, m_snd_driver_globals, SndDriverGlobals);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_FX, m_fx, FxEffectDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_IMPACT_FX, m_fx_impact_table, FxImpactTable);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_RAWFILE, m_raw_file, RawFile);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_STRINGTABLE, m_string_table, StringTable);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_LEADERBOARD, m_leaderboard, LeaderboardDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_XGLOBALS, m_xglobals, XGlobals);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_DDL, m_ddl, ddlRoot_t);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_GLASSES, m_glasses, Glasses);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_EMBLEMSET, m_emblem_set, EmblemSet);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SCRIPTPARSETREE, m_script, ScriptParseTree);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_KEYVALUEPAIRS, m_key_value_pairs, KeyValuePairs);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_VEHICLEDEF, m_vehicle, VehicleDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MEMORYBLOCK, m_memory_block, MemoryBlock);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_ADDON_MAP_ENTS, m_addon_map_ents, AddonMapEnts);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_TRACER, m_tracer, TracerDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SKINNEDVERTS, m_skinned_verts, SkinnedVertsDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_QDB, m_qdb, Qdb);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SLUG, m_slug, Slug);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_FOOTSTEP_TABLE, m_footstep_table, FootstepTableDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_FOOTSTEPFX_TABLE, m_footstep_fx_table, FootstepFXTableDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_ZBARRIER, m_zbarrier, ZBarrierDef);

    default:
        assert(type >= 0 && type < ASSET_TYPE_COUNT);
        break;
    }

#undef CASE_INIT_POOL_STATIC
}

void GameAssetPoolT6::InitPoolDynamic(const asset_type_t type)
{
#define CASE_INIT_POOL_DYNAMIC(assetType, poolName, poolType) \
    case assetType: \
    { \
        if((poolName) == nullptr) \
        { \
            (poolName) = new AssetPoolDynamic<poolType>(m_priority); \
        } \
        break; \
    }

    switch(type)
    {
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_PHYSPRESET, m_phys_preset, PhysPreset);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_PHYSCONSTRAINTS, m_phys_constraints, PhysConstraints);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_DESTRUCTIBLEDEF, m_destructible_def, DestructibleDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_XANIMPARTS, m_xanim_parts, XAnimParts);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_XMODEL, m_xmodel, XModel);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MATERIAL, m_material, Material);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_TECHNIQUE_SET, m_technique_set, MaterialTechniqueSet);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_IMAGE, m_image, GfxImage);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SOUND, m_sound_bank, SndBank);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SOUND_PATCH, m_sound_patch, SndPatch);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_CLIPMAP, m_clip_map, clipMap_t);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_CLIPMAP_PVS, m_clip_map, clipMap_t);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_COMWORLD, m_com_world, ComWorld);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_GAMEWORLD_SP, m_game_world_sp, GameWorldSp);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_GAMEWORLD_MP, m_game_world_mp, GameWorldMp);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MAP_ENTS, m_map_ents, MapEnts);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_GFXWORLD, m_gfx_world, GfxWorld);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def, GfxLightDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_FONT, m_font, Font_s);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_FONTICON, m_font_icon, FontIcon);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MENULIST, m_menu_list, MenuList);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MENU, m_menu_def, menuDef_t);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_LOCALIZE_ENTRY, m_localize, LocalizeEntry);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_WEAPON, m_weapon, WeaponVariantDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_ATTACHMENT, m_attachment, WeaponAttachment);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_ATTACHMENT_UNIQUE, m_attachment_unique, WeaponAttachmentUnique);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_WEAPON_CAMO, m_camo, WeaponCamo);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SNDDRIVER_GLOBALS, m_snd_driver_globals, SndDriverGlobals);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_FX, m_fx, FxEffectDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_IMPACT_FX, m_fx_impact_table, FxImpactTable);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_RAWFILE, m_raw_file, RawFile);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_STRINGTABLE, m_string_table, StringTable);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_LEADERBOARD, m_leaderboard, LeaderboardDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_XGLOBALS, m_xglobals, XGlobals);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_DDL, m_ddl, ddlRoot_t);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_GLASSES, m_glasses, Glasses);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_EMBLEMSET, m_emblem_set, EmblemSet);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SCRIPTPARSETREE, m_script, ScriptParseTree);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_KEYVALUEPAIRS, m_key_value_pairs, KeyValuePairs);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_VEHICLEDEF, m_vehicle, VehicleDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MEMORYBLOCK, m_memory_block, MemoryBlock);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_ADDON_MAP_ENTS, m_addon_map_ents, AddonMapEnts);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_TRACER, m_tracer, TracerDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SKINNEDVERTS, m_skinned_verts, SkinnedVertsDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_QDB, m_qdb, Qdb);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SLUG, m_slug, Slug);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_FOOTSTEP_TABLE, m_footstep_table, FootstepTableDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_FOOTSTEPFX_TABLE, m_footstep_fx_table, FootstepFXTableDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_ZBARRIER, m_zbarrier, ZBarrierDef);

    default:
        assert(type >= 0 && type < ASSET_TYPE_COUNT);
        break;
    }

#undef CASE_INIT_POOL_STATIC
}

void* GameAssetPoolT6::AddAsset(asset_type_t type, std::string name, void* asset, std::vector<std::string>& scriptStrings, std::vector<XAssetDependency>& dependencies)
{
    XAsset xAsset{};

    xAsset.type = static_cast<XAssetType>(type);
    xAsset.header.data = asset;

#define CASE_ADD_TO_POOL(assetType, poolName, headerName) \
    case assetType: \
    { \
        assert((poolName) != nullptr); \
        return (poolName)->AddAsset(std::move(name), xAsset.header.headerName, scriptStrings, dependencies)->m_asset; \
    }

    switch(xAsset.type)
    {
        CASE_ADD_TO_POOL(ASSET_TYPE_PHYSPRESET, m_phys_preset, physPreset);
        CASE_ADD_TO_POOL(ASSET_TYPE_PHYSCONSTRAINTS, m_phys_constraints, physConstraints);
        CASE_ADD_TO_POOL(ASSET_TYPE_DESTRUCTIBLEDEF, m_destructible_def, destructibleDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_XANIMPARTS, m_xanim_parts, parts);
        CASE_ADD_TO_POOL(ASSET_TYPE_XMODEL, m_xmodel, model);
        CASE_ADD_TO_POOL(ASSET_TYPE_MATERIAL, m_material, material);
        CASE_ADD_TO_POOL(ASSET_TYPE_TECHNIQUE_SET, m_technique_set, techniqueSet);
        CASE_ADD_TO_POOL(ASSET_TYPE_IMAGE, m_image, image);
        CASE_ADD_TO_POOL(ASSET_TYPE_SOUND, m_sound_bank, sound);
        CASE_ADD_TO_POOL(ASSET_TYPE_SOUND_PATCH, m_sound_patch, soundPatch);
        CASE_ADD_TO_POOL(ASSET_TYPE_CLIPMAP, m_clip_map, clipMap);
        CASE_ADD_TO_POOL(ASSET_TYPE_CLIPMAP_PVS, m_clip_map, clipMap);
        CASE_ADD_TO_POOL(ASSET_TYPE_COMWORLD, m_com_world, comWorld);
        CASE_ADD_TO_POOL(ASSET_TYPE_GAMEWORLD_SP, m_game_world_sp, gameWorldSp);
        CASE_ADD_TO_POOL(ASSET_TYPE_GAMEWORLD_MP, m_game_world_mp, gameWorldMp);
        CASE_ADD_TO_POOL(ASSET_TYPE_MAP_ENTS, m_map_ents, mapEnts);
        CASE_ADD_TO_POOL(ASSET_TYPE_GFXWORLD, m_gfx_world, gfxWorld);
        CASE_ADD_TO_POOL(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def, lightDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_FONT, m_font, font);
        CASE_ADD_TO_POOL(ASSET_TYPE_FONTICON, m_font_icon, fontIcon);
        CASE_ADD_TO_POOL(ASSET_TYPE_MENULIST, m_menu_list, menuList);
        CASE_ADD_TO_POOL(ASSET_TYPE_MENU, m_menu_def, menu);
        CASE_ADD_TO_POOL(ASSET_TYPE_LOCALIZE_ENTRY, m_localize, localize);
        CASE_ADD_TO_POOL(ASSET_TYPE_WEAPON, m_weapon, weapon);
        CASE_ADD_TO_POOL(ASSET_TYPE_ATTACHMENT, m_attachment, attachment);
        CASE_ADD_TO_POOL(ASSET_TYPE_ATTACHMENT_UNIQUE, m_attachment_unique, attachmentUnique);
        CASE_ADD_TO_POOL(ASSET_TYPE_WEAPON_CAMO, m_camo, weaponCamo);
        CASE_ADD_TO_POOL(ASSET_TYPE_SNDDRIVER_GLOBALS, m_snd_driver_globals, sndDriverGlobals);
        CASE_ADD_TO_POOL(ASSET_TYPE_FX, m_fx, fx);
        CASE_ADD_TO_POOL(ASSET_TYPE_IMPACT_FX, m_fx_impact_table, impactFx);
        CASE_ADD_TO_POOL(ASSET_TYPE_RAWFILE, m_raw_file, rawfile);
        CASE_ADD_TO_POOL(ASSET_TYPE_STRINGTABLE, m_string_table, stringTable);
        CASE_ADD_TO_POOL(ASSET_TYPE_LEADERBOARD, m_leaderboard, leaderboardDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_XGLOBALS, m_xglobals, xGlobals);
        CASE_ADD_TO_POOL(ASSET_TYPE_DDL, m_ddl, ddlRoot);
        CASE_ADD_TO_POOL(ASSET_TYPE_GLASSES, m_glasses, glasses);
        CASE_ADD_TO_POOL(ASSET_TYPE_EMBLEMSET, m_emblem_set, emblemSet);
        CASE_ADD_TO_POOL(ASSET_TYPE_SCRIPTPARSETREE, m_script, scriptParseTree);
        CASE_ADD_TO_POOL(ASSET_TYPE_KEYVALUEPAIRS, m_key_value_pairs, keyValuePairs);
        CASE_ADD_TO_POOL(ASSET_TYPE_VEHICLEDEF, m_vehicle, vehicleDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_MEMORYBLOCK, m_memory_block, memoryBlock);
        CASE_ADD_TO_POOL(ASSET_TYPE_ADDON_MAP_ENTS, m_addon_map_ents, addonMapEnts);
        CASE_ADD_TO_POOL(ASSET_TYPE_TRACER, m_tracer, tracerDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_SKINNEDVERTS, m_skinned_verts, skinnedVertsDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_QDB, m_qdb, qdb);
        CASE_ADD_TO_POOL(ASSET_TYPE_SLUG, m_slug, slug);
        CASE_ADD_TO_POOL(ASSET_TYPE_FOOTSTEP_TABLE, m_footstep_table, footstepTableDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_FOOTSTEPFX_TABLE, m_footstep_fx_table, footstepFXTableDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_ZBARRIER, m_zbarrier, zbarrierDef);

    default:
        assert(false);
        break;
    }

    return nullptr;

#undef CASE_ADD_TO_POOL
}

ZoneContent GameAssetPoolT6::GetContent() const
{
    ZoneContent content{};

    content.m_game_name = "T6";

#define POOL_ADD_TO_CONTENT(assetTypeName, poolName) \
    if((poolName) != nullptr) \
    { \
        for(auto asset : *(poolName)) \
        { \
            content.m_assets.emplace_back((assetTypeName), asset->m_name);\
        } \
    }

    POOL_ADD_TO_CONTENT("physpreset", m_phys_preset);
    POOL_ADD_TO_CONTENT("physconstraints", m_phys_constraints);
    POOL_ADD_TO_CONTENT("destructibledef", m_destructible_def);
    POOL_ADD_TO_CONTENT("xanim", m_xanim_parts);
    POOL_ADD_TO_CONTENT("xmodel", m_xmodel);
    POOL_ADD_TO_CONTENT("material", m_material);
    POOL_ADD_TO_CONTENT("techniqueset", m_technique_set);
    POOL_ADD_TO_CONTENT("image", m_image);
    POOL_ADD_TO_CONTENT("soundbank", m_sound_bank);
    POOL_ADD_TO_CONTENT("soundpatch", m_sound_patch);
    POOL_ADD_TO_CONTENT("clipmap", m_clip_map);
    POOL_ADD_TO_CONTENT("comworld", m_com_world);
    POOL_ADD_TO_CONTENT("gameworldsp", m_game_world_sp);
    POOL_ADD_TO_CONTENT("gameworldmp", m_game_world_mp);
    POOL_ADD_TO_CONTENT("mapents", m_map_ents);
    POOL_ADD_TO_CONTENT("gfxworld", m_gfx_world);
    POOL_ADD_TO_CONTENT("gfxlightdef", m_gfx_light_def);
    POOL_ADD_TO_CONTENT("font", m_font);
    POOL_ADD_TO_CONTENT("fonticon", m_font_icon);
    POOL_ADD_TO_CONTENT("menulist", m_menu_list);
    POOL_ADD_TO_CONTENT("menudef", m_menu_def);
    POOL_ADD_TO_CONTENT("localize", m_localize);
    POOL_ADD_TO_CONTENT("weapon", m_weapon);
    POOL_ADD_TO_CONTENT("attachment", m_attachment);
    POOL_ADD_TO_CONTENT("attachmentunique", m_attachment_unique);
    POOL_ADD_TO_CONTENT("camo", m_camo);
    POOL_ADD_TO_CONTENT("snddriverglobals", m_snd_driver_globals);
    POOL_ADD_TO_CONTENT("fx", m_fx);
    POOL_ADD_TO_CONTENT("fximpacttable", m_fx_impact_table);
    POOL_ADD_TO_CONTENT("rawfile", m_raw_file);
    POOL_ADD_TO_CONTENT("stringtable", m_string_table);
    POOL_ADD_TO_CONTENT("leaderboard", m_leaderboard);
    POOL_ADD_TO_CONTENT("xglobals", m_xglobals);
    POOL_ADD_TO_CONTENT("ddl", m_ddl);
    POOL_ADD_TO_CONTENT("glasses", m_glasses);
    POOL_ADD_TO_CONTENT("emblemset", m_emblem_set);
    POOL_ADD_TO_CONTENT("script", m_script);
    POOL_ADD_TO_CONTENT("keyvaluepairs", m_key_value_pairs);
    POOL_ADD_TO_CONTENT("vehicle", m_vehicle);
    POOL_ADD_TO_CONTENT("memoryblock", m_memory_block);
    POOL_ADD_TO_CONTENT("addonmapents", m_addon_map_ents);
    POOL_ADD_TO_CONTENT("tracer", m_tracer);
    POOL_ADD_TO_CONTENT("skinnedverts", m_skinned_verts);
    POOL_ADD_TO_CONTENT("qdb", m_qdb);
    POOL_ADD_TO_CONTENT("slug", m_slug);
    POOL_ADD_TO_CONTENT("footsteptable", m_footstep_table);
    POOL_ADD_TO_CONTENT("footstepfxtable", m_footstep_fx_table);
    POOL_ADD_TO_CONTENT("zbarrier", m_zbarrier);

    return content;

#undef POOL_ADD_TO_CONTENT
}