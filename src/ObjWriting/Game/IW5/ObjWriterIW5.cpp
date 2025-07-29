#include "ObjWriterIW5.h"

#include "Game/IW5/GameAssetPoolIW5.h"
#include "Game/IW5/Material/MaterialJsonDumperIW5.h"
#include "Game/IW5/XModel/XModelDumperIW5.h"
#include "Image/ImageDumperIW5.h"
#include "Leaderboard/LeaderboardJsonDumperIW5.h"
#include "Localize/LocalizeDumperIW5.h"
#include "Maps/AddonMapEntsDumperIW5.h"
#include "Menu/AssetDumperMenuDef.h"
#include "Menu/AssetDumperMenuList.h"
#include "ObjWriting.h"
#include "RawFile/AssetDumperRawFile.h"
#include "Script/AssetDumperScriptFile.h"
#include "Sound/AssetDumperLoadedSound.h"
#include "StringTable/AssetDumperStringTable.h"
#include "Weapon/AssetDumperWeapon.h"
#include "Weapon/AssetDumperWeaponAttachment.h"

using namespace IW5;

bool ObjWriter::DumpZone(AssetDumpingContext& context) const
{
#define DUMP_ASSET_POOL(dumperType, poolName, assetType)                                                                                                       \
    if (assetPools->poolName && ObjWriting::ShouldHandleAssetType(assetType))                                                                                  \
    {                                                                                                                                                          \
        dumperType dumper;                                                                                                                                     \
        dumper.DumpPool(context, assetPools->poolName.get());                                                                                                  \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolIW5*>(context.m_zone.m_pools.get());
    // DUMP_ASSET_POOL(AssetDumperPhysPreset, m_phys_preset, ASSET_TYPE_PHYSPRESET)
    // DUMP_ASSET_POOL(AssetDumperPhysCollmap, m_phys_collmap, ASSET_TYPE_PHYSCOLLMAP)
    // DUMP_ASSET_POOL(AssetDumperXAnimParts, m_xanim_parts, ASSET_TYPE_XANIMPARTS)
    // DUMP_ASSET_POOL(AssetDumperXModelSurfs, m_xmodel_surfs, ASSET_TYPE_XMODEL_SURFS)
    DUMP_ASSET_POOL(AssetDumperXModel, m_xmodel, ASSET_TYPE_XMODEL)
    DUMP_ASSET_POOL(material::JsonDumper, m_material, ASSET_TYPE_MATERIAL)
    // DUMP_ASSET_POOL(AssetDumperMaterialPixelShader, m_material_pixel_shader, ASSET_TYPE_PIXELSHADER)
    // DUMP_ASSET_POOL(AssetDumperMaterialVertexShader, m_material_vertex_shader, ASSET_TYPE_VERTEXSHADER)
    // DUMP_ASSET_POOL(AssetDumperMaterialVertexDeclaration, m_material_vertex_decl, ASSET_TYPE_VERTEXDECL)
    // DUMP_ASSET_POOL(AssetDumperMaterialTechniqueSet, m_technique_set, ASSET_TYPE_TECHNIQUE_SET)
    DUMP_ASSET_POOL(image::Dumper, m_image, ASSET_TYPE_IMAGE)
    // DUMP_ASSET_POOL(AssetDumpersnd_alias_list_t, m_sound, ASSET_TYPE_SOUND)
    // DUMP_ASSET_POOL(AssetDumperSndCurve, m_sound_curve, ASSET_TYPE_SOUND_CURVE)
    DUMP_ASSET_POOL(AssetDumperLoadedSound, m_loaded_sound, ASSET_TYPE_LOADED_SOUND)
    // DUMP_ASSET_POOL(AssetDumperclipMap_t, m_clip_map, ASSET_TYPE_CLIPMAP)
    // DUMP_ASSET_POOL(AssetDumperComWorld, m_com_world, ASSET_TYPE_COMWORLD)
    // DUMP_ASSET_POOL(AssetDumperGlassWorld, m_glass_world, ASSET_TYPE_GLASSWORLD)
    // DUMP_ASSET_POOL(AssetDumperPathData, m_path_data, ASSET_TYPE_PATHDATA)
    // DUMP_ASSET_POOL(AssetDumperVehicleTrack, m_vehicle_track, ASSET_TYPE_VEHICLE_TRACK)
    // DUMP_ASSET_POOL(AssetDumperMapEnts, m_map_ents, ASSET_TYPE_MAP_ENTS)
    // DUMP_ASSET_POOL(AssetDumperFxWorld, m_fx_world, ASSET_TYPE_FXWORLD)
    // DUMP_ASSET_POOL(AssetDumperGfxWorld, m_gfx_world, ASSET_TYPE_GFXWORLD)
    // DUMP_ASSET_POOL(AssetDumperGfxLightDef, m_gfx_light_def, ASSET_TYPE_LIGHT_DEF)
    // DUMP_ASSET_POOL(AssetDumperFont_s, m_font, ASSET_TYPE_FONT)
    DUMP_ASSET_POOL(AssetDumperMenuList, m_menu_list, ASSET_TYPE_MENULIST)
    DUMP_ASSET_POOL(AssetDumperMenuDef, m_menu_def, ASSET_TYPE_MENU)
    DUMP_ASSET_POOL(localize::Dumper, m_localize, ASSET_TYPE_LOCALIZE_ENTRY)
    DUMP_ASSET_POOL(AssetDumperWeaponAttachment, m_attachment, ASSET_TYPE_ATTACHMENT)
    DUMP_ASSET_POOL(AssetDumperWeapon, m_weapon, ASSET_TYPE_WEAPON)
    // DUMP_ASSET_POOL(AssetDumperFxEffectDef, m_fx, ASSET_TYPE_FX)
    // DUMP_ASSET_POOL(AssetDumperFxImpactTable, m_fx_impact_table, ASSET_TYPE_IMPACT_FX)
    // DUMP_ASSET_POOL(AssetDumperSurfaceFxTable, m_surface_fx_table, ASSET_TYPE_SURFACE_FX)
    DUMP_ASSET_POOL(AssetDumperRawFile, m_raw_file, ASSET_TYPE_RAWFILE)
    DUMP_ASSET_POOL(AssetDumperScriptFile, m_script_file, ASSET_TYPE_SCRIPTFILE)
    DUMP_ASSET_POOL(AssetDumperStringTable, m_string_table, ASSET_TYPE_STRINGTABLE)
    DUMP_ASSET_POOL(leaderboard::JsonDumper, m_leaderboard, ASSET_TYPE_LEADERBOARD)
    // DUMP_ASSET_POOL(AssetDumperStructuredDataDefSet, m_structed_data_def_set, ASSET_TYPE_STRUCTURED_DATA_DEF)
    // DUMP_ASSET_POOL(AssetDumperTracerDef, m_tracer, ASSET_TYPE_TRACER)
    // DUMP_ASSET_POOL(AssetDumperVehicleDef, m_vehicle, ASSET_TYPE_VEHICLE)
    DUMP_ASSET_POOL(addon_map_ents::Dumper, m_addon_map_ents, ASSET_TYPE_ADDON_MAP_ENTS)

    return true;

#undef DUMP_ASSET_POOL
}
