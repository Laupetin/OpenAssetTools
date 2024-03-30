#include "GameAssetPoolIW5.h"

#include "Pool/AssetPoolDynamic.h"
#include "Pool/AssetPoolStatic.h"

#include <cassert>
#include <type_traits>

using namespace IW5;

const char* GameAssetPoolIW5::ASSET_TYPE_NAMES[]{
    "physpreset",
    "physcollmap",
    "xanim",
    "xmodelsurfs",
    "xmodel",
    "material",
    "pixelshader",
    "vertexshader",
    "vertexdecl",
    "techniqueset",
    "image",
    "sound",
    "soundcurve",
    "loadedsound",
    "clipmap",
    "comworld",
    "glassworld",
    "pathdata",
    "vehicletrack",
    "mapents",
    "fxworld",
    "gfxworld",
    "lightdef",
    "uimap",
    "font",
    "menulist",
    "menu",
    "localize",
    "attachment",
    "weapon",
    "snddriverglobals",
    "fx",
    "impactfx",
    "surfacefx",
    "aitype",
    "mptype",
    "character",
    "xmodelalias",
    "rawfile",
    "scriptfile",
    "stringtable",
    "leaderboard",
    "structureddatadef",
    "tracer",
    "vehicle",
    "addonmapents",
};

GameAssetPoolIW5::GameAssetPoolIW5(Zone* zone, const int priority)
    : ZoneAssetPools(zone),
      m_priority(priority)
{
    assert(std::extent_v<decltype(ASSET_TYPE_NAMES)> == ASSET_TYPE_COUNT);
}

void GameAssetPoolIW5::InitPoolStatic(const asset_type_t type, const size_t capacity)
{
#define CASE_INIT_POOL_STATIC(assetType, poolName)                                                                                                             \
    case assetType:                                                                                                                                            \
    {                                                                                                                                                          \
        if ((poolName) == nullptr && capacity > 0)                                                                                                             \
        {                                                                                                                                                      \
            (poolName) = std::make_unique<AssetPoolStatic<decltype(poolName)::element_type::type>>(capacity, m_priority, (assetType));                         \
        }                                                                                                                                                      \
        break;                                                                                                                                                 \
    }

    switch (type)
    {
        CASE_INIT_POOL_STATIC(ASSET_TYPE_PHYSPRESET, m_phys_preset)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_PHYSCOLLMAP, m_phys_collmap)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_XANIMPARTS, m_xanim_parts)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_XMODEL_SURFS, m_xmodel_surfs)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_XMODEL, m_xmodel)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MATERIAL, m_material)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_PIXELSHADER, m_material_pixel_shader)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_VERTEXSHADER, m_material_vertex_shader)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_VERTEXDECL, m_material_vertex_decl)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_TECHNIQUE_SET, m_technique_set)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_IMAGE, m_image)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SOUND, m_sound)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SOUND_CURVE, m_sound_curve)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_LOADED_SOUND, m_loaded_sound)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_CLIPMAP, m_clip_map)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_COMWORLD, m_com_world)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_GLASSWORLD, m_glass_world)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_PATHDATA, m_path_data)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_VEHICLE_TRACK, m_vehicle_track)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MAP_ENTS, m_map_ents)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_FXWORLD, m_fx_world)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_GFXWORLD, m_gfx_world)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_FONT, m_font)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MENULIST, m_menu_list)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MENU, m_menu_def)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_LOCALIZE_ENTRY, m_localize)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_ATTACHMENT, m_attachment)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_WEAPON, m_weapon)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_FX, m_fx)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_IMPACT_FX, m_fx_impact_table)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SURFACE_FX, m_surface_fx_table)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_RAWFILE, m_raw_file)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SCRIPTFILE, m_script_file)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_STRINGTABLE, m_string_table)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_LEADERBOARD, m_leaderboard)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_STRUCTURED_DATA_DEF, m_structed_data_def_set)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_TRACER, m_tracer)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_VEHICLE, m_vehicle)
        CASE_INIT_POOL_STATIC(ASSET_TYPE_ADDON_MAP_ENTS, m_addon_map_ents)

    default:
        assert(type >= 0 && type < ASSET_TYPE_COUNT);
        break;
    }

#undef CASE_INIT_POOL_STATIC
}

void GameAssetPoolIW5::InitPoolDynamic(const asset_type_t type)
{
#define CASE_INIT_POOL_DYNAMIC(assetType, poolName)                                                                                                            \
    case assetType:                                                                                                                                            \
    {                                                                                                                                                          \
        if ((poolName) == nullptr)                                                                                                                             \
        {                                                                                                                                                      \
            (poolName) = std::make_unique<AssetPoolDynamic<decltype(poolName)::element_type::type>>(m_priority, (assetType));                                  \
        }                                                                                                                                                      \
        break;                                                                                                                                                 \
    }

    switch (type)
    {
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_PHYSPRESET, m_phys_preset)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_PHYSCOLLMAP, m_phys_collmap)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_XANIMPARTS, m_xanim_parts)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_XMODEL_SURFS, m_xmodel_surfs)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_XMODEL, m_xmodel)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MATERIAL, m_material)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_PIXELSHADER, m_material_pixel_shader)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_VERTEXSHADER, m_material_vertex_shader)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_VERTEXDECL, m_material_vertex_decl)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_TECHNIQUE_SET, m_technique_set)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_IMAGE, m_image)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SOUND, m_sound)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SOUND_CURVE, m_sound_curve)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_LOADED_SOUND, m_loaded_sound)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_CLIPMAP, m_clip_map)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_COMWORLD, m_com_world)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_GLASSWORLD, m_glass_world)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_PATHDATA, m_path_data)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_VEHICLE_TRACK, m_vehicle_track)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MAP_ENTS, m_map_ents)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_FXWORLD, m_fx_world)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_GFXWORLD, m_gfx_world)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_FONT, m_font)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MENULIST, m_menu_list)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MENU, m_menu_def)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_LOCALIZE_ENTRY, m_localize)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_ATTACHMENT, m_attachment)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_WEAPON, m_weapon)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_FX, m_fx)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_IMPACT_FX, m_fx_impact_table)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SURFACE_FX, m_surface_fx_table)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_RAWFILE, m_raw_file)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SCRIPTFILE, m_script_file)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_STRINGTABLE, m_string_table)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_LEADERBOARD, m_leaderboard)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_STRUCTURED_DATA_DEF, m_structed_data_def_set)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_TRACER, m_tracer)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_VEHICLE, m_vehicle)
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_ADDON_MAP_ENTS, m_addon_map_ents)

    default:
        assert(type >= 0 && type < ASSET_TYPE_COUNT);
        break;
    }

#undef CASE_INIT_POOL_DYNAMIC
}

XAssetInfoGeneric* GameAssetPoolIW5::AddAssetToPool(std::unique_ptr<XAssetInfoGeneric> xAssetInfo)
{
#define CASE_ADD_TO_POOL(assetType, poolName)                                                                                                                  \
    case assetType:                                                                                                                                            \
    {                                                                                                                                                          \
        assert((poolName) != nullptr);                                                                                                                         \
        return (poolName)->AddAsset(std::unique_ptr<XAssetInfo<decltype(poolName)::element_type::type>>(                                                       \
            static_cast<XAssetInfo<decltype(poolName)::element_type::type>*>(xAssetInfo.release())));                                                          \
    }

    switch (static_cast<XAssetType>(xAssetInfo->m_type))
    {
        CASE_ADD_TO_POOL(ASSET_TYPE_PHYSPRESET, m_phys_preset)
        CASE_ADD_TO_POOL(ASSET_TYPE_PHYSCOLLMAP, m_phys_collmap)
        CASE_ADD_TO_POOL(ASSET_TYPE_XANIMPARTS, m_xanim_parts)
        CASE_ADD_TO_POOL(ASSET_TYPE_XMODEL_SURFS, m_xmodel_surfs)
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
        CASE_ADD_TO_POOL(ASSET_TYPE_CLIPMAP, m_clip_map)
        CASE_ADD_TO_POOL(ASSET_TYPE_COMWORLD, m_com_world)
        CASE_ADD_TO_POOL(ASSET_TYPE_GLASSWORLD, m_glass_world)
        CASE_ADD_TO_POOL(ASSET_TYPE_PATHDATA, m_path_data)
        CASE_ADD_TO_POOL(ASSET_TYPE_VEHICLE_TRACK, m_vehicle_track)
        CASE_ADD_TO_POOL(ASSET_TYPE_MAP_ENTS, m_map_ents)
        CASE_ADD_TO_POOL(ASSET_TYPE_FXWORLD, m_fx_world)
        CASE_ADD_TO_POOL(ASSET_TYPE_GFXWORLD, m_gfx_world)
        CASE_ADD_TO_POOL(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def)
        CASE_ADD_TO_POOL(ASSET_TYPE_FONT, m_font)
        CASE_ADD_TO_POOL(ASSET_TYPE_MENULIST, m_menu_list)
        CASE_ADD_TO_POOL(ASSET_TYPE_MENU, m_menu_def)
        CASE_ADD_TO_POOL(ASSET_TYPE_LOCALIZE_ENTRY, m_localize)
        CASE_ADD_TO_POOL(ASSET_TYPE_ATTACHMENT, m_attachment)
        CASE_ADD_TO_POOL(ASSET_TYPE_WEAPON, m_weapon)
        CASE_ADD_TO_POOL(ASSET_TYPE_FX, m_fx)
        CASE_ADD_TO_POOL(ASSET_TYPE_IMPACT_FX, m_fx_impact_table)
        CASE_ADD_TO_POOL(ASSET_TYPE_SURFACE_FX, m_surface_fx_table)
        CASE_ADD_TO_POOL(ASSET_TYPE_RAWFILE, m_raw_file)
        CASE_ADD_TO_POOL(ASSET_TYPE_SCRIPTFILE, m_script_file)
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

XAssetInfoGeneric* GameAssetPoolIW5::GetAsset(const asset_type_t type, const std::string& name) const
{
#define CASE_GET_ASSET(assetType, poolName)                                                                                                                    \
    case assetType:                                                                                                                                            \
    {                                                                                                                                                          \
        if ((poolName) != nullptr)                                                                                                                             \
            return (poolName)->GetAsset(std::move(name));                                                                                                      \
        break;                                                                                                                                                 \
    }

    switch (type)
    {
        CASE_GET_ASSET(ASSET_TYPE_PHYSPRESET, m_phys_preset)
        CASE_GET_ASSET(ASSET_TYPE_PHYSCOLLMAP, m_phys_collmap)
        CASE_GET_ASSET(ASSET_TYPE_XANIMPARTS, m_xanim_parts)
        CASE_GET_ASSET(ASSET_TYPE_XMODEL_SURFS, m_xmodel_surfs)
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
        CASE_GET_ASSET(ASSET_TYPE_CLIPMAP, m_clip_map)
        CASE_GET_ASSET(ASSET_TYPE_COMWORLD, m_com_world)
        CASE_GET_ASSET(ASSET_TYPE_GLASSWORLD, m_glass_world)
        CASE_GET_ASSET(ASSET_TYPE_PATHDATA, m_path_data)
        CASE_GET_ASSET(ASSET_TYPE_VEHICLE_TRACK, m_vehicle_track)
        CASE_GET_ASSET(ASSET_TYPE_MAP_ENTS, m_map_ents)
        CASE_GET_ASSET(ASSET_TYPE_FXWORLD, m_fx_world)
        CASE_GET_ASSET(ASSET_TYPE_GFXWORLD, m_gfx_world)
        CASE_GET_ASSET(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def)
        CASE_GET_ASSET(ASSET_TYPE_FONT, m_font)
        CASE_GET_ASSET(ASSET_TYPE_MENULIST, m_menu_list)
        CASE_GET_ASSET(ASSET_TYPE_MENU, m_menu_def)
        CASE_GET_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, m_localize)
        CASE_GET_ASSET(ASSET_TYPE_ATTACHMENT, m_attachment)
        CASE_GET_ASSET(ASSET_TYPE_WEAPON, m_weapon)
        CASE_GET_ASSET(ASSET_TYPE_FX, m_fx)
        CASE_GET_ASSET(ASSET_TYPE_IMPACT_FX, m_fx_impact_table)
        CASE_GET_ASSET(ASSET_TYPE_SURFACE_FX, m_surface_fx_table)
        CASE_GET_ASSET(ASSET_TYPE_RAWFILE, m_raw_file)
        CASE_GET_ASSET(ASSET_TYPE_SCRIPTFILE, m_script_file)
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

const char* GameAssetPoolIW5::AssetTypeNameByType(const asset_type_t assetType)
{
    if (assetType >= 0 && assetType < static_cast<int>(std::extent_v<decltype(ASSET_TYPE_NAMES)>))
        return ASSET_TYPE_NAMES[assetType];

    return ASSET_TYPE_INVALID;
}

const char* GameAssetPoolIW5::GetAssetTypeName(const asset_type_t assetType) const
{
    return AssetTypeNameByType(assetType);
}

asset_type_t GameAssetPoolIW5::AssetTypeCount()
{
    return ASSET_TYPE_COUNT;
}

asset_type_t GameAssetPoolIW5::GetAssetTypeCount() const
{
    return AssetTypeCount();
}
