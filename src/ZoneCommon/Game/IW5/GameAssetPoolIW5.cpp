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

/*
    Asset Pool Table
    Useful for macro generation via regex-replace for example

    #assetType, #typeName, #unionEntry, #poolName
    ASSET_TYPE_PHYSPRESET, PhysPreset, physPreset, m_phys_preset
    ASSET_TYPE_PHYSCOLLMAP, PhysCollmap, physCollmap, m_phys_collmap
    ASSET_TYPE_XANIMPARTS, XAnimParts, parts, m_xanim_parts
    ASSET_TYPE_XMODEL_SURFS, XModelSurfs, modelSurfs, m_xmodel_surfs
    ASSET_TYPE_XMODEL, XModel, model, m_xmodel
    ASSET_TYPE_MATERIAL, Material, material, m_material
    ASSET_TYPE_PIXELSHADER, MaterialPixelShader, pixelShader, m_material_pixel_shader
    ASSET_TYPE_VERTEXSHADER, MaterialVertexShader, vertexShader, m_material_vertex_shader
    ASSET_TYPE_VERTEXDECL, MaterialVertexDeclaration, vertexDecl, m_material_vertex_decl
    ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet, techniqueSet, m_technique_set
    ASSET_TYPE_IMAGE, GfxImage, image, m_image
    ASSET_TYPE_SOUND, snd_alias_list_t, sound, m_sound
    ASSET_TYPE_SOUND_CURVE, SndCurve, sndCurve, m_sound_curve
    ASSET_TYPE_LOADED_SOUND, LoadedSound, loadSnd, m_loaded_sound
    ASSET_TYPE_CLIPMAP, clipMap_t, clipMap, m_clip_map
    ASSET_TYPE_COMWORLD, ComWorld, comWorld, m_com_world
    ASSET_TYPE_GLASSWORLD, GlassWorld, glassWorld, m_glass_world
    ASSET_TYPE_PATHDATA, PathData, pathData, m_path_data
    ASSET_TYPE_VEHICLE_TRACK, VehicleTrack, vehicleTrack, m_vehicle_track
    ASSET_TYPE_MAP_ENTS, MapEnts, mapEnts, m_map_ents
    ASSET_TYPE_FXWORLD, FxWorld, fxWorld, m_fx_world
    ASSET_TYPE_GFXWORLD, GfxWorld, gfxWorld, m_gfx_world
    ASSET_TYPE_LIGHT_DEF, GfxLightDef, lightDef, m_gfx_light_def
    ASSET_TYPE_FONT, Font_s, font, m_font
    ASSET_TYPE_MENULIST, MenuList, menuList, m_menu_list
    ASSET_TYPE_MENU, menuDef_t, menu, m_menu_def
    ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry, localize, m_localize
    ASSET_TYPE_ATTACHMENT, WeaponAttachment, attachment, m_attachment
    ASSET_TYPE_WEAPON, WeaponCompleteDef, weapon, m_weapon
    ASSET_TYPE_FX, FxEffectDef, fx, m_fx
    ASSET_TYPE_IMPACT_FX, FxImpactTable, impactFx, m_fx_impact_table
    ASSET_TYPE_SURFACE_FX, SurfaceFxTable, surfaceFx, m_surface_fx_table
    ASSET_TYPE_RAWFILE, RawFile, rawfile, m_raw_file
    ASSET_TYPE_SCRIPTFILE, ScriptFile, scriptfile, m_script_file
    ASSET_TYPE_STRINGTABLE, StringTable, stringTable, m_string_table
    ASSET_TYPE_LEADERBOARD, LeaderboardDef, leaderboardDef, m_leaderboard
    ASSET_TYPE_STRUCTURED_DATA_DEF, StructuredDataDefSet, structuredDataDefSet, m_structed_data_def_set
    ASSET_TYPE_TRACER, TracerDef, tracerDef, m_tracer
    ASSET_TYPE_VEHICLE, VehicleDef, vehDef, m_vehicle
    ASSET_TYPE_ADDON_MAP_ENTS, AddonMapEnts, addonMapEnts, m_addon_map_ents
 */

GameAssetPoolIW5::GameAssetPoolIW5(Zone* zone, const int priority)
    : ZoneAssetPools(zone),
      m_priority(priority)
{
    assert(std::extent<decltype(ASSET_TYPE_NAMES)>::value == ASSET_TYPE_COUNT);
}

void GameAssetPoolIW5::InitPoolStatic(const asset_type_t type, const size_t capacity)
{
#define CASE_INIT_POOL_STATIC(assetType, poolName, poolType)                                                                                                   \
    case assetType:                                                                                                                                            \
    {                                                                                                                                                          \
        if ((poolName) == nullptr && capacity > 0)                                                                                                             \
        {                                                                                                                                                      \
            (poolName) = std::make_unique<AssetPoolStatic<poolType>>(capacity, m_priority, (assetType));                                                       \
        }                                                                                                                                                      \
        break;                                                                                                                                                 \
    }

    switch (type)
    {
        CASE_INIT_POOL_STATIC(ASSET_TYPE_PHYSPRESET, m_phys_preset, PhysPreset);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_PHYSCOLLMAP, m_phys_collmap, PhysCollmap);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_XANIMPARTS, m_xanim_parts, XAnimParts);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_XMODEL_SURFS, m_xmodel_surfs, XModelSurfs);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_XMODEL, m_xmodel, XModel);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MATERIAL, m_material, Material);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_PIXELSHADER, m_material_pixel_shader, MaterialPixelShader);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_VERTEXSHADER, m_material_vertex_shader, MaterialVertexShader);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_VERTEXDECL, m_material_vertex_decl, MaterialVertexDeclaration);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_TECHNIQUE_SET, m_technique_set, MaterialTechniqueSet);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_IMAGE, m_image, GfxImage);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SOUND, m_sound, snd_alias_list_t);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SOUND_CURVE, m_sound_curve, SndCurve);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_LOADED_SOUND, m_loaded_sound, LoadedSound);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_CLIPMAP, m_clip_map, clipMap_t);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_COMWORLD, m_com_world, ComWorld);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_GLASSWORLD, m_glass_world, GlassWorld);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_PATHDATA, m_path_data, PathData);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_VEHICLE_TRACK, m_vehicle_track, VehicleTrack);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MAP_ENTS, m_map_ents, MapEnts);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_FXWORLD, m_fx_world, FxWorld);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_GFXWORLD, m_gfx_world, GfxWorld);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def, GfxLightDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_FONT, m_font, Font_s);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MENULIST, m_menu_list, MenuList);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_MENU, m_menu_def, menuDef_t);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_LOCALIZE_ENTRY, m_localize, LocalizeEntry);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_ATTACHMENT, m_attachment, WeaponAttachment);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_WEAPON, m_weapon, WeaponCompleteDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_FX, m_fx, FxEffectDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_IMPACT_FX, m_fx_impact_table, FxImpactTable);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SURFACE_FX, m_surface_fx_table, SurfaceFxTable);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_RAWFILE, m_raw_file, RawFile);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_SCRIPTFILE, m_script_file, ScriptFile);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_STRINGTABLE, m_string_table, StringTable);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_LEADERBOARD, m_leaderboard, LeaderboardDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_STRUCTURED_DATA_DEF, m_structed_data_def_set, StructuredDataDefSet);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_TRACER, m_tracer, TracerDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_VEHICLE, m_vehicle, VehicleDef);
        CASE_INIT_POOL_STATIC(ASSET_TYPE_ADDON_MAP_ENTS, m_addon_map_ents, AddonMapEnts);

    default:
        assert(type >= 0 && type < ASSET_TYPE_COUNT);
        break;
    }

#undef CASE_INIT_POOL_STATIC
}

void GameAssetPoolIW5::InitPoolDynamic(const asset_type_t type)
{
#define CASE_INIT_POOL_DYNAMIC(assetType, poolName, poolType)                                                                                                  \
    case assetType:                                                                                                                                            \
    {                                                                                                                                                          \
        if ((poolName) == nullptr)                                                                                                                             \
        {                                                                                                                                                      \
            (poolName) = std::make_unique<AssetPoolDynamic<poolType>>(m_priority, (assetType));                                                                \
        }                                                                                                                                                      \
        break;                                                                                                                                                 \
    }

    switch (type)
    {
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_PHYSPRESET, m_phys_preset, PhysPreset);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_PHYSCOLLMAP, m_phys_collmap, PhysCollmap);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_XANIMPARTS, m_xanim_parts, XAnimParts);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_XMODEL_SURFS, m_xmodel_surfs, XModelSurfs);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_XMODEL, m_xmodel, XModel);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MATERIAL, m_material, Material);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_PIXELSHADER, m_material_pixel_shader, MaterialPixelShader);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_VERTEXSHADER, m_material_vertex_shader, MaterialVertexShader);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_VERTEXDECL, m_material_vertex_decl, MaterialVertexDeclaration);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_TECHNIQUE_SET, m_technique_set, MaterialTechniqueSet);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_IMAGE, m_image, GfxImage);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SOUND, m_sound, snd_alias_list_t);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SOUND_CURVE, m_sound_curve, SndCurve);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_LOADED_SOUND, m_loaded_sound, LoadedSound);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_CLIPMAP, m_clip_map, clipMap_t);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_COMWORLD, m_com_world, ComWorld);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_GLASSWORLD, m_glass_world, GlassWorld);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_PATHDATA, m_path_data, PathData);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_VEHICLE_TRACK, m_vehicle_track, VehicleTrack);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MAP_ENTS, m_map_ents, MapEnts);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_FXWORLD, m_fx_world, FxWorld);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_GFXWORLD, m_gfx_world, GfxWorld);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def, GfxLightDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_FONT, m_font, Font_s);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MENULIST, m_menu_list, MenuList);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_MENU, m_menu_def, menuDef_t);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_LOCALIZE_ENTRY, m_localize, LocalizeEntry);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_ATTACHMENT, m_attachment, WeaponAttachment);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_WEAPON, m_weapon, WeaponCompleteDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_FX, m_fx, FxEffectDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_IMPACT_FX, m_fx_impact_table, FxImpactTable);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SURFACE_FX, m_surface_fx_table, SurfaceFxTable);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_RAWFILE, m_raw_file, RawFile);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_SCRIPTFILE, m_script_file, ScriptFile);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_STRINGTABLE, m_string_table, StringTable);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_LEADERBOARD, m_leaderboard, LeaderboardDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_STRUCTURED_DATA_DEF, m_structed_data_def_set, StructuredDataDefSet);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_TRACER, m_tracer, TracerDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_VEHICLE, m_vehicle, VehicleDef);
        CASE_INIT_POOL_DYNAMIC(ASSET_TYPE_ADDON_MAP_ENTS, m_addon_map_ents, AddonMapEnts);

    default:
        assert(type >= 0 && type < ASSET_TYPE_COUNT);
        break;
    }

#undef CASE_INIT_POOL_STATIC
}

XAssetInfoGeneric* GameAssetPoolIW5::AddAssetToPool(
    asset_type_t type, std::string name, void* asset, std::vector<XAssetInfoGeneric*> dependencies, std::vector<scr_string_t> usedScriptStrings, Zone* zone)
{
    XAsset xAsset{};

    xAsset.type = static_cast<XAssetType>(type);
    xAsset.header.data = asset;

#define CASE_ADD_TO_POOL(assetType, poolName, headerName)                                                                                                      \
    case assetType:                                                                                                                                            \
    {                                                                                                                                                          \
        assert((poolName) != nullptr);                                                                                                                         \
        return (poolName)->AddAsset(std::move(name), xAsset.header.headerName, zone, std::move(dependencies), std::move(usedScriptStrings));                   \
    }

    switch (xAsset.type)
    {
        CASE_ADD_TO_POOL(ASSET_TYPE_PHYSPRESET, m_phys_preset, physPreset);
        CASE_ADD_TO_POOL(ASSET_TYPE_PHYSCOLLMAP, m_phys_collmap, physCollmap);
        CASE_ADD_TO_POOL(ASSET_TYPE_XANIMPARTS, m_xanim_parts, parts);
        CASE_ADD_TO_POOL(ASSET_TYPE_XMODEL_SURFS, m_xmodel_surfs, modelSurfs);
        CASE_ADD_TO_POOL(ASSET_TYPE_XMODEL, m_xmodel, model);
        CASE_ADD_TO_POOL(ASSET_TYPE_MATERIAL, m_material, material);
        CASE_ADD_TO_POOL(ASSET_TYPE_PIXELSHADER, m_material_pixel_shader, pixelShader);
        CASE_ADD_TO_POOL(ASSET_TYPE_VERTEXSHADER, m_material_vertex_shader, vertexShader);
        CASE_ADD_TO_POOL(ASSET_TYPE_VERTEXDECL, m_material_vertex_decl, vertexDecl);
        CASE_ADD_TO_POOL(ASSET_TYPE_TECHNIQUE_SET, m_technique_set, techniqueSet);
        CASE_ADD_TO_POOL(ASSET_TYPE_IMAGE, m_image, image);
        CASE_ADD_TO_POOL(ASSET_TYPE_SOUND, m_sound, sound);
        CASE_ADD_TO_POOL(ASSET_TYPE_SOUND_CURVE, m_sound_curve, sndCurve);
        CASE_ADD_TO_POOL(ASSET_TYPE_LOADED_SOUND, m_loaded_sound, loadSnd);
        CASE_ADD_TO_POOL(ASSET_TYPE_CLIPMAP, m_clip_map, clipMap);
        CASE_ADD_TO_POOL(ASSET_TYPE_COMWORLD, m_com_world, comWorld);
        CASE_ADD_TO_POOL(ASSET_TYPE_GLASSWORLD, m_glass_world, glassWorld);
        CASE_ADD_TO_POOL(ASSET_TYPE_PATHDATA, m_path_data, pathData);
        CASE_ADD_TO_POOL(ASSET_TYPE_VEHICLE_TRACK, m_vehicle_track, vehicleTrack);
        CASE_ADD_TO_POOL(ASSET_TYPE_MAP_ENTS, m_map_ents, mapEnts);
        CASE_ADD_TO_POOL(ASSET_TYPE_FXWORLD, m_fx_world, fxWorld);
        CASE_ADD_TO_POOL(ASSET_TYPE_GFXWORLD, m_gfx_world, gfxWorld);
        CASE_ADD_TO_POOL(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def, lightDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_FONT, m_font, font);
        CASE_ADD_TO_POOL(ASSET_TYPE_MENULIST, m_menu_list, menuList);
        CASE_ADD_TO_POOL(ASSET_TYPE_MENU, m_menu_def, menu);
        CASE_ADD_TO_POOL(ASSET_TYPE_LOCALIZE_ENTRY, m_localize, localize);
        CASE_ADD_TO_POOL(ASSET_TYPE_ATTACHMENT, m_attachment, attachment);
        CASE_ADD_TO_POOL(ASSET_TYPE_WEAPON, m_weapon, weapon);
        CASE_ADD_TO_POOL(ASSET_TYPE_FX, m_fx, fx);
        CASE_ADD_TO_POOL(ASSET_TYPE_IMPACT_FX, m_fx_impact_table, impactFx);
        CASE_ADD_TO_POOL(ASSET_TYPE_SURFACE_FX, m_surface_fx_table, surfaceFx);
        CASE_ADD_TO_POOL(ASSET_TYPE_RAWFILE, m_raw_file, rawfile);
        CASE_ADD_TO_POOL(ASSET_TYPE_SCRIPTFILE, m_script_file, scriptfile);
        CASE_ADD_TO_POOL(ASSET_TYPE_STRINGTABLE, m_string_table, stringTable);
        CASE_ADD_TO_POOL(ASSET_TYPE_LEADERBOARD, m_leaderboard, leaderboardDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_STRUCTURED_DATA_DEF, m_structed_data_def_set, structuredDataDefSet);
        CASE_ADD_TO_POOL(ASSET_TYPE_TRACER, m_tracer, tracerDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_VEHICLE, m_vehicle, vehDef);
        CASE_ADD_TO_POOL(ASSET_TYPE_ADDON_MAP_ENTS, m_addon_map_ents, addonMapEnts);

    default:
        assert(false);
        break;
    }

    return nullptr;

#undef CASE_ADD_TO_POOL
}

XAssetInfoGeneric* GameAssetPoolIW5::GetAsset(const asset_type_t type, std::string name) const
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
        CASE_GET_ASSET(ASSET_TYPE_PHYSPRESET, m_phys_preset);
        CASE_GET_ASSET(ASSET_TYPE_PHYSCOLLMAP, m_phys_collmap);
        CASE_GET_ASSET(ASSET_TYPE_XANIMPARTS, m_xanim_parts);
        CASE_GET_ASSET(ASSET_TYPE_XMODEL_SURFS, m_xmodel_surfs);
        CASE_GET_ASSET(ASSET_TYPE_XMODEL, m_xmodel);
        CASE_GET_ASSET(ASSET_TYPE_MATERIAL, m_material);
        CASE_GET_ASSET(ASSET_TYPE_PIXELSHADER, m_material_pixel_shader);
        CASE_GET_ASSET(ASSET_TYPE_VERTEXSHADER, m_material_vertex_shader);
        CASE_GET_ASSET(ASSET_TYPE_VERTEXDECL, m_material_vertex_decl);
        CASE_GET_ASSET(ASSET_TYPE_TECHNIQUE_SET, m_technique_set);
        CASE_GET_ASSET(ASSET_TYPE_IMAGE, m_image);
        CASE_GET_ASSET(ASSET_TYPE_SOUND, m_sound);
        CASE_GET_ASSET(ASSET_TYPE_SOUND_CURVE, m_sound_curve);
        CASE_GET_ASSET(ASSET_TYPE_LOADED_SOUND, m_loaded_sound);
        CASE_GET_ASSET(ASSET_TYPE_CLIPMAP, m_clip_map);
        CASE_GET_ASSET(ASSET_TYPE_COMWORLD, m_com_world);
        CASE_GET_ASSET(ASSET_TYPE_GLASSWORLD, m_glass_world);
        CASE_GET_ASSET(ASSET_TYPE_PATHDATA, m_path_data);
        CASE_GET_ASSET(ASSET_TYPE_VEHICLE_TRACK, m_vehicle_track);
        CASE_GET_ASSET(ASSET_TYPE_MAP_ENTS, m_map_ents);
        CASE_GET_ASSET(ASSET_TYPE_FXWORLD, m_fx_world);
        CASE_GET_ASSET(ASSET_TYPE_GFXWORLD, m_gfx_world);
        CASE_GET_ASSET(ASSET_TYPE_LIGHT_DEF, m_gfx_light_def);
        CASE_GET_ASSET(ASSET_TYPE_FONT, m_font);
        CASE_GET_ASSET(ASSET_TYPE_MENULIST, m_menu_list);
        CASE_GET_ASSET(ASSET_TYPE_MENU, m_menu_def);
        CASE_GET_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, m_localize);
        CASE_GET_ASSET(ASSET_TYPE_ATTACHMENT, m_attachment);
        CASE_GET_ASSET(ASSET_TYPE_WEAPON, m_weapon);
        CASE_GET_ASSET(ASSET_TYPE_FX, m_fx);
        CASE_GET_ASSET(ASSET_TYPE_IMPACT_FX, m_fx_impact_table);
        CASE_GET_ASSET(ASSET_TYPE_SURFACE_FX, m_surface_fx_table);
        CASE_GET_ASSET(ASSET_TYPE_RAWFILE, m_raw_file);
        CASE_GET_ASSET(ASSET_TYPE_SCRIPTFILE, m_script_file);
        CASE_GET_ASSET(ASSET_TYPE_STRINGTABLE, m_string_table);
        CASE_GET_ASSET(ASSET_TYPE_LEADERBOARD, m_leaderboard);
        CASE_GET_ASSET(ASSET_TYPE_STRUCTURED_DATA_DEF, m_structed_data_def_set);
        CASE_GET_ASSET(ASSET_TYPE_TRACER, m_tracer);
        CASE_GET_ASSET(ASSET_TYPE_VEHICLE, m_vehicle);
        CASE_GET_ASSET(ASSET_TYPE_ADDON_MAP_ENTS, m_addon_map_ents);

    default:
        assert(false);
        break;
    }

    return nullptr;

#undef CASE_GET_ASSET
}

const char* GameAssetPoolIW5::AssetTypeNameByType(asset_type_t assetType)
{
    if (assetType >= 0 && assetType < static_cast<int>(std::extent<decltype(ASSET_TYPE_NAMES)>::value))
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
