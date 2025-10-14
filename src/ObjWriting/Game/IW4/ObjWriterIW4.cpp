#include "ObjWriterIW4.h"

#include "Game/IW4/GameAssetPoolIW4.h"
#include "Game/IW4/Material/MaterialJsonDumperIW4.h"
#include "Game/IW4/XModel/XModelDumperIW4.h"
#include "Image/ImageDumperIW4.h"
#include "Leaderboard/LeaderboardJsonDumperIW4.h"
#include "LightDef/LightDefDumperIW4.h"
#include "Localize/LocalizeDumperIW4.h"
#include "Maps/AddonMapEntsDumperIW4.h"
#include "Material/MaterialDecompilingDumperIW4.h"
#include "Menu/MenuDumperIW4.h"
#include "Menu/MenuListDumperIW4.h"
#include "ObjWriting.h"
#include "PhysCollmap/PhysCollmapDumperIW4.h"
#include "PhysPreset/PhysPresetInfoStringDumperIW4.h"
#include "RawFile/RawFileDumperIW4.h"
#include "Shader/PixelShaderDumperIW4.h"
#include "Shader/VertexShaderDumperIW4.h"
#include "Sound/LoadedSoundDumperIW4.h"
#include "Sound/SndCurveDumperIW4.h"
#include "StringTable/StringTableDumperIW4.h"
#include "StructuredDataDef/StructuredDataDefDumperIW4.h"
#include "Techset/TechsetDumperIW4.h"
#include "Tracer/TracerDumperIW4.h"
#include "Vehicle/VehicleDumperIW4.h"
#include "Weapon/WeaponDumperIW4.h"

using namespace IW4;

bool ObjWriter::DumpZone(AssetDumpingContext& context) const
{
#define DUMP_ASSET_POOL(dumperType, poolName, assetType)                                                                                                       \
    if (assetPools->poolName && ObjWriting::ShouldHandleAssetType(assetType))                                                                                  \
    {                                                                                                                                                          \
        dumperType dumper;                                                                                                                                     \
        totalProgress += dumper.GetProgressTotalCount(*assetPools->poolName);                                                                                  \
        dumpingFunctions.emplace_back(                                                                                                                         \
            [](AssetDumpingContext& funcContext, const GameAssetPoolIW4* funcPools)                                                                             \
            {                                                                                                                                                  \
                dumperType dumper;                                                                                                                             \
                dumper.DumpPool(funcContext, *funcPools->poolName);                                                                                            \
            });                                                                                                                                                \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolIW4*>(context.m_zone.m_pools.get());

    size_t totalProgress = 0uz;
    std::vector<std::function<void(AssetDumpingContext & context, const GameAssetPoolIW4*)>> dumpingFunctions;

    DUMP_ASSET_POOL(phys_preset::InfoStringDumperIW4, m_phys_preset, ASSET_TYPE_PHYSPRESET)
    DUMP_ASSET_POOL(phys_collmap::DumperIW4, m_phys_collmap, ASSET_TYPE_PHYSCOLLMAP)
    // DUMP_ASSET_POOL(AssetDumperXAnimParts, m_xanim_parts, ASSET_TYPE_XANIMPARTS)
    DUMP_ASSET_POOL(xmodel::DumperIW4, m_xmodel, ASSET_TYPE_XMODEL)
    DUMP_ASSET_POOL(material::JsonDumperIW4, m_material, ASSET_TYPE_MATERIAL)
#ifdef EXPERIMENTAL_MATERIAL_COMPILATION
    DUMP_ASSET_POOL(material::DecompilingGdtDumperIW4, m_material, ASSET_TYPE_MATERIAL)
#endif
    DUMP_ASSET_POOL(shader::PixelShaderDumperIW4, m_material_pixel_shader, ASSET_TYPE_PIXELSHADER)
    DUMP_ASSET_POOL(shader::VertexShaderDumperIW4, m_material_vertex_shader, ASSET_TYPE_VERTEXSHADER)
    DUMP_ASSET_POOL(techset::DumperIW4, m_technique_set, ASSET_TYPE_TECHNIQUE_SET)
    DUMP_ASSET_POOL(image::DumperIW4, m_image, ASSET_TYPE_IMAGE)
    // DUMP_ASSET_POOL(AssetDumpersnd_alias_list_t, m_sound, ASSET_TYPE_SOUND)
    DUMP_ASSET_POOL(sound_curve::DumperIW4, m_sound_curve, ASSET_TYPE_SOUND_CURVE)
    DUMP_ASSET_POOL(sound::LoadedSoundDumperIW4, m_loaded_sound, ASSET_TYPE_LOADED_SOUND)
    // DUMP_ASSET_POOL(AssetDumperClipMap, m_clip_map, ASSET_TYPE_CLIPMAP_MP)
    // DUMP_ASSET_POOL(AssetDumperComWorld, m_com_world, ASSET_TYPE_COMWORLD)
    // DUMP_ASSET_POOL(AssetDumperGameWorldSp, m_game_world_sp, ASSET_TYPE_GAMEWORLD_SP)
    // DUMP_ASSET_POOL(AssetDumperGameWorldMp, m_game_world_mp, ASSET_TYPE_GAMEWORLD_MP)
    // DUMP_ASSET_POOL(AssetDumperMapEnts, m_map_ents, ASSET_TYPE_MAP_ENTS)
    // DUMP_ASSET_POOL(AssetDumperFxWorld, m_fx_world, ASSET_TYPE_FXWORLD)
    // DUMP_ASSET_POOL(AssetDumperGfxWorld, m_gfx_world, ASSET_TYPE_GFXWORLD)
    DUMP_ASSET_POOL(light_def::DumperIW4, m_gfx_light_def, ASSET_TYPE_LIGHT_DEF)
    // DUMP_ASSET_POOL(AssetDumperFont_s, m_font, ASSET_TYPE_FONT)
    DUMP_ASSET_POOL(menu::MenuListDumperIW4, m_menu_list, ASSET_TYPE_MENULIST)
    DUMP_ASSET_POOL(menu::MenuDumperIW4, m_menu_def, ASSET_TYPE_MENU)
    DUMP_ASSET_POOL(localize::DumperIW4, m_localize, ASSET_TYPE_LOCALIZE_ENTRY)
    DUMP_ASSET_POOL(weapon::DumperIW4, m_weapon, ASSET_TYPE_WEAPON)
    // DUMP_ASSET_POOL(AssetDumperSndDriverGlobals, m_snd_driver_globals, ASSET_TYPE_SNDDRIVER_GLOBALS)
    // DUMP_ASSET_POOL(AssetDumperFxEffectDef, m_fx, ASSET_TYPE_FX)
    // DUMP_ASSET_POOL(AssetDumperFxImpactTable, m_fx_impact_table, ASSET_TYPE_IMPACT_FX)
    DUMP_ASSET_POOL(raw_file::DumperIW4, m_raw_file, ASSET_TYPE_RAWFILE)
    DUMP_ASSET_POOL(string_table::DumperIW4, m_string_table, ASSET_TYPE_STRINGTABLE)
    DUMP_ASSET_POOL(leaderboard::JsonDumperIW4, m_leaderboard, ASSET_TYPE_LEADERBOARD)
    DUMP_ASSET_POOL(structured_data_def::DumperIW4, m_structed_data_def_set, ASSET_TYPE_STRUCTURED_DATA_DEF)
    DUMP_ASSET_POOL(tracer::DumperIW4, m_tracer, ASSET_TYPE_TRACER)
    DUMP_ASSET_POOL(vehicle::DumperIW4, m_vehicle, ASSET_TYPE_VEHICLE)
    DUMP_ASSET_POOL(addon_map_ents::DumperIW4, m_addon_map_ents, ASSET_TYPE_ADDON_MAP_ENTS)

    context.SetTotalProgress(totalProgress);
    for (const auto& func : dumpingFunctions)
        func(context, assetPools);

    return true;

#undef DUMP_ASSET_POOL
}
