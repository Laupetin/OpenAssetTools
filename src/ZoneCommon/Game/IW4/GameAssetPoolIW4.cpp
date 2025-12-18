#include "GameAssetPoolIW4.h"

#include "Pool/AssetPoolDynamic.h"

#include <cassert>
#include <type_traits>

using namespace IW4;

namespace
{
    constexpr const char* ASSET_TYPE_NAMES[]{
        "physpreset",  "physcollmap", "xanim",        "xmodelsurfs", "xmodel",
        "material",    "pixelshader", "vertexshader", "vertexdecl",  "techniqueset",
        "image",       "sound",       "soundcurve",   "loadedsound", "clipmap_unused",
        "clipmap",     "comworld",    "gameworldsp",  "gameworldmp", "mapents",
        "fxworld",     "gfxworld",    "lightdef",     "uimap",       "font",
        "menulist",    "menu",        "localize",     "weapon",      "snddriverglobals",
        "fx",          "impactfx",    "aitype",       "mptype",      "character",
        "xmodelalias", "rawfile",     "stringtable",  "leaderboard", "structureddatadef",
        "tracer",      "vehicle",     "addonmapents",
    };
}

GameAssetPoolIW4::GameAssetPoolIW4(Zone* zone, const zone_priority_t priority)
    : ZoneAssetPools(zone),
      m_priority(priority)
{
    static_assert(std::extent_v<decltype(ASSET_TYPE_NAMES)> == ASSET_TYPE_COUNT);

#define INIT_POOL(poolName) (poolName) = std::make_unique<AssetPoolDynamic<decltype(poolName)::element_type::type>>(m_priority)

    INIT_POOL(m_phys_preset);
    INIT_POOL(m_phys_collmap);
    INIT_POOL(m_xanim_parts);
    INIT_POOL(m_xmodel);
    INIT_POOL(m_material);
    INIT_POOL(m_material_pixel_shader);
    INIT_POOL(m_material_vertex_shader);
    INIT_POOL(m_material_vertex_decl);
    INIT_POOL(m_technique_set);
    INIT_POOL(m_image);
    INIT_POOL(m_sound);
    INIT_POOL(m_sound_curve);
    INIT_POOL(m_loaded_sound);
    INIT_POOL(m_clip_map);
    INIT_POOL(m_com_world);
    INIT_POOL(m_game_world_sp);
    INIT_POOL(m_game_world_mp);
    INIT_POOL(m_map_ents);
    INIT_POOL(m_fx_world);
    INIT_POOL(m_gfx_world);
    INIT_POOL(m_gfx_light_def);
    INIT_POOL(m_font);
    INIT_POOL(m_menu_list);
    INIT_POOL(m_menu_def);
    INIT_POOL(m_localize);
    INIT_POOL(m_weapon);
    INIT_POOL(m_fx);
    INIT_POOL(m_fx_impact_table);
    INIT_POOL(m_raw_file);
    INIT_POOL(m_string_table);
    INIT_POOL(m_leaderboard);
    INIT_POOL(m_structed_data_def_set);
    INIT_POOL(m_tracer);
    INIT_POOL(m_vehicle);
    INIT_POOL(m_addon_map_ents);

#undef INIT_POOL
}

XAssetInfoGeneric* GameAssetPoolIW4::AddAssetToPool(std::unique_ptr<XAssetInfoGeneric> xAssetInfo)
{
#define CASE_ADD_TO_POOL(assetType, poolName)                                                                                                                  \
    case assetType:                                                                                                                                            \
    {                                                                                                                                                          \
        assert(poolName);                                                                                                                                      \
        return (poolName)->AddAsset(std::unique_ptr<XAssetInfo<decltype(poolName)::element_type::type>>(                                                       \
            static_cast<XAssetInfo<decltype(poolName)::element_type::type>*>(xAssetInfo.release())));                                                          \
    }

    switch (static_cast<XAssetType>(xAssetInfo->m_type))
    {
        CASE_ADD_TO_POOL(ASSET_TYPE_PHYSPRESET, m_phys_preset)
        CASE_ADD_TO_POOL(ASSET_TYPE_PHYSCOLLMAP, m_phys_collmap)
        CASE_ADD_TO_POOL(ASSET_TYPE_XANIMPARTS, m_xanim_parts)
        CASE_ADD_TO_POOL(ASSET_TYPE_XMODEL, m_xmodel)
        CASE_ADD_TO_POOL(ASSET_TYPE_MATERIAL, m_material)
        CASE_ADD_TO_POOL(ASSET_TYPE_PIXELSHADER, m_material_pixel_shader)
        CASE_ADD_TO_POOL(ASSET_TYPE_VERTEXSHADER, m_material_vertex_shader)
        CASE_ADD_TO_POOL(ASSET_TYPE_VERTEXDECL, m_material_vertex_decl)
        CASE_ADD_TO_POOL(ASSET_TYPE_TECHNIQUE_SET, m_technique_set)
        CASE_ADD_TO_POOL(ASSET_TYPE_IMAGE, m_image)
        CASE_ADD_TO_POOL(ASSET_TYPE_SOUND, m_sound)
        CASE_ADD_TO_POOL(ASSET_TYPE_SOUND_CURVE, m_sound_curve)
        CASE_ADD_TO_POOL(ASSET_TYPE_LOADED_SOUND, m_loaded_sound)
        CASE_ADD_TO_POOL(ASSET_TYPE_CLIPMAP_SP, m_clip_map)
        CASE_ADD_TO_POOL(ASSET_TYPE_CLIPMAP_MP, m_clip_map)
        CASE_ADD_TO_POOL(ASSET_TYPE_COMWORLD, m_com_world)
        CASE_ADD_TO_POOL(ASSET_TYPE_GAMEWORLD_SP, m_game_world_sp)
        CASE_ADD_TO_POOL(ASSET_TYPE_GAMEWORLD_MP, m_game_world_mp)
        CASE_ADD_TO_POOL(ASSET_TYPE_MAP_ENTS, m_map_ents)
        CASE_ADD_TO_POOL(ASSET_TYPE_FXWORLD, m_fx_world)
        CASE_ADD_TO_POOL(ASSET_TYPE_GFXWORLD, m_gfx_world)
        CASE_ADD_TO_POOL(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def)
        CASE_ADD_TO_POOL(ASSET_TYPE_FONT, m_font)
        CASE_ADD_TO_POOL(ASSET_TYPE_MENULIST, m_menu_list)
        CASE_ADD_TO_POOL(ASSET_TYPE_MENU, m_menu_def)
        CASE_ADD_TO_POOL(ASSET_TYPE_LOCALIZE_ENTRY, m_localize)
        CASE_ADD_TO_POOL(ASSET_TYPE_WEAPON, m_weapon)
        CASE_ADD_TO_POOL(ASSET_TYPE_FX, m_fx)
        CASE_ADD_TO_POOL(ASSET_TYPE_IMPACT_FX, m_fx_impact_table)
        CASE_ADD_TO_POOL(ASSET_TYPE_RAWFILE, m_raw_file)
        CASE_ADD_TO_POOL(ASSET_TYPE_STRINGTABLE, m_string_table)
        CASE_ADD_TO_POOL(ASSET_TYPE_LEADERBOARD, m_leaderboard)
        CASE_ADD_TO_POOL(ASSET_TYPE_STRUCTURED_DATA_DEF, m_structed_data_def_set)
        CASE_ADD_TO_POOL(ASSET_TYPE_TRACER, m_tracer)
        CASE_ADD_TO_POOL(ASSET_TYPE_VEHICLE, m_vehicle)
        CASE_ADD_TO_POOL(ASSET_TYPE_ADDON_MAP_ENTS, m_addon_map_ents)

    default:
        assert(false);
        break;
    }

    return nullptr;

#undef CASE_ADD_TO_POOL
}

XAssetInfoGeneric* GameAssetPoolIW4::GetAsset(const asset_type_t type, const std::string& name) const
{
#define CASE_GET_ASSET(assetType, poolName)                                                                                                                    \
    case assetType:                                                                                                                                            \
    {                                                                                                                                                          \
        if (poolName)                                                                                                                                          \
            return (poolName)->GetAsset(name);                                                                                                                 \
        break;                                                                                                                                                 \
    }

    switch (type)
    {
        CASE_GET_ASSET(ASSET_TYPE_PHYSPRESET, m_phys_preset)
        CASE_GET_ASSET(ASSET_TYPE_PHYSCOLLMAP, m_phys_collmap)
        CASE_GET_ASSET(ASSET_TYPE_XANIMPARTS, m_xanim_parts)
        CASE_GET_ASSET(ASSET_TYPE_XMODEL, m_xmodel)
        CASE_GET_ASSET(ASSET_TYPE_MATERIAL, m_material)
        CASE_GET_ASSET(ASSET_TYPE_PIXELSHADER, m_material_pixel_shader)
        CASE_GET_ASSET(ASSET_TYPE_VERTEXSHADER, m_material_vertex_shader)
        CASE_GET_ASSET(ASSET_TYPE_VERTEXDECL, m_material_vertex_decl)
        CASE_GET_ASSET(ASSET_TYPE_TECHNIQUE_SET, m_technique_set)
        CASE_GET_ASSET(ASSET_TYPE_IMAGE, m_image)
        CASE_GET_ASSET(ASSET_TYPE_SOUND, m_sound)
        CASE_GET_ASSET(ASSET_TYPE_SOUND_CURVE, m_sound_curve)
        CASE_GET_ASSET(ASSET_TYPE_LOADED_SOUND, m_loaded_sound)
        CASE_GET_ASSET(ASSET_TYPE_CLIPMAP_SP, m_clip_map)
        CASE_GET_ASSET(ASSET_TYPE_CLIPMAP_MP, m_clip_map)
        CASE_GET_ASSET(ASSET_TYPE_COMWORLD, m_com_world)
        CASE_GET_ASSET(ASSET_TYPE_GAMEWORLD_SP, m_game_world_sp)
        CASE_GET_ASSET(ASSET_TYPE_GAMEWORLD_MP, m_game_world_mp)
        CASE_GET_ASSET(ASSET_TYPE_MAP_ENTS, m_map_ents)
        CASE_GET_ASSET(ASSET_TYPE_FXWORLD, m_fx_world)
        CASE_GET_ASSET(ASSET_TYPE_GFXWORLD, m_gfx_world)
        CASE_GET_ASSET(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def)
        CASE_GET_ASSET(ASSET_TYPE_FONT, m_font)
        CASE_GET_ASSET(ASSET_TYPE_MENULIST, m_menu_list)
        CASE_GET_ASSET(ASSET_TYPE_MENU, m_menu_def)
        CASE_GET_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, m_localize)
        CASE_GET_ASSET(ASSET_TYPE_WEAPON, m_weapon)
        CASE_GET_ASSET(ASSET_TYPE_FX, m_fx)
        CASE_GET_ASSET(ASSET_TYPE_IMPACT_FX, m_fx_impact_table)
        CASE_GET_ASSET(ASSET_TYPE_RAWFILE, m_raw_file)
        CASE_GET_ASSET(ASSET_TYPE_STRINGTABLE, m_string_table)
        CASE_GET_ASSET(ASSET_TYPE_LEADERBOARD, m_leaderboard)
        CASE_GET_ASSET(ASSET_TYPE_STRUCTURED_DATA_DEF, m_structed_data_def_set)
        CASE_GET_ASSET(ASSET_TYPE_TRACER, m_tracer)
        CASE_GET_ASSET(ASSET_TYPE_VEHICLE, m_vehicle)
        CASE_GET_ASSET(ASSET_TYPE_ADDON_MAP_ENTS, m_addon_map_ents)

    default:
        assert(false);
        break;
    }

    return nullptr;

#undef CASE_GET_ASSET
}

std::optional<const char*> GameAssetPoolIW4::AssetTypeNameByType(const asset_type_t assetType)
{
    if (assetType >= 0 && assetType < static_cast<int>(std::extent_v<decltype(ASSET_TYPE_NAMES)>))
        return ASSET_TYPE_NAMES[assetType];

    return std::nullopt;
}

std::optional<const char*> GameAssetPoolIW4::GetAssetTypeName(const asset_type_t assetType) const
{
    return AssetTypeNameByType(assetType);
}

asset_type_t GameAssetPoolIW4::AssetTypeCount()
{
    return ASSET_TYPE_COUNT;
}

asset_type_t GameAssetPoolIW4::GetAssetTypeCount() const
{
    return AssetTypeCount();
}
