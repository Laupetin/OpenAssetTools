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
#define REGISTER_DUMPER(dumperType, poolName)                                                                                                                  \
    if (assetPools->poolName && ObjWriting::ShouldHandleAssetType(dumperType::AssetType_t::EnumEntry))                                                         \
    {                                                                                                                                                          \
        dumpers.emplace_back(std::make_unique<dumperType>(*assetPools->poolName));                                                                             \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolIW4*>(context.m_zone.m_pools.get());
    std::vector<std::unique_ptr<IAssetDumper>> dumpers;

    REGISTER_DUMPER(phys_preset::InfoStringDumperIW4, m_phys_preset)
    REGISTER_DUMPER(phys_collmap::DumperIW4, m_phys_collmap)
    // REGISTER_DUMPER(AssetDumperXAnimParts, m_xanim_parts)
    REGISTER_DUMPER(xmodel::DumperIW4, m_xmodel)
    REGISTER_DUMPER(material::JsonDumperIW4, m_material)
#ifdef EXPERIMENTAL_MATERIAL_COMPILATION
    DUMP_ASSET_POOL(material::DecompilingGdtDumperIW4, m_material)
#endif
    REGISTER_DUMPER(shader::PixelShaderDumperIW4, m_material_pixel_shader)
    REGISTER_DUMPER(shader::VertexShaderDumperIW4, m_material_vertex_shader)
    REGISTER_DUMPER(techset::DumperIW4, m_technique_set)
    REGISTER_DUMPER(image::DumperIW4, m_image)
    // REGISTER_DUMPER(AssetDumpersnd_alias_list_t, m_sound)
    REGISTER_DUMPER(sound_curve::DumperIW4, m_sound_curve)
    REGISTER_DUMPER(sound::LoadedSoundDumperIW4, m_loaded_sound)
    // REGISTER_DUMPER(AssetDumperClipMap, m_clip_map)
    // REGISTER_DUMPER(AssetDumperComWorld, m_com_world)
    // REGISTER_DUMPER(AssetDumperGameWorldSp, m_game_world_sp)
    // REGISTER_DUMPER(AssetDumperGameWorldMp, m_game_world_mp)
    // REGISTER_DUMPER(AssetDumperMapEnts, m_map_ents)
    // REGISTER_DUMPER(AssetDumperFxWorld, m_fx_world)
    // REGISTER_DUMPER(AssetDumperGfxWorld, m_gfx_world)
    REGISTER_DUMPER(light_def::DumperIW4, m_gfx_light_def)
    // REGISTER_DUMPER(AssetDumperFont_s, m_font)
    REGISTER_DUMPER(menu::MenuListDumperIW4, m_menu_list)
    REGISTER_DUMPER(menu::MenuDumperIW4, m_menu_def)
    REGISTER_DUMPER(localize::DumperIW4, m_localize)
    REGISTER_DUMPER(weapon::DumperIW4, m_weapon)
    // REGISTER_DUMPER(AssetDumperSndDriverGlobals, m_snd_driver_globals)
    // REGISTER_DUMPER(AssetDumperFxEffectDef, m_fx)
    // REGISTER_DUMPER(AssetDumperFxImpactTable, m_fx_impact_table)
    REGISTER_DUMPER(raw_file::DumperIW4, m_raw_file)
    REGISTER_DUMPER(string_table::DumperIW4, m_string_table)
    REGISTER_DUMPER(leaderboard::JsonDumperIW4, m_leaderboard)
    REGISTER_DUMPER(structured_data_def::DumperIW4, m_structed_data_def_set)
    REGISTER_DUMPER(tracer::DumperIW4, m_tracer)
    REGISTER_DUMPER(vehicle::DumperIW4, m_vehicle)
    REGISTER_DUMPER(addon_map_ents::DumperIW4, m_addon_map_ents)

    if (context.ShouldTrackProgress())
    {
        size_t totalProgress = 0uz;
        for (const auto& dumper : dumpers)
            totalProgress += dumper->GetProgressTotalCount();

        context.SetTotalProgress(totalProgress);
    }

    for (const auto& dumper : dumpers)
        dumper->Dump(context);

    return true;

#undef REGISTER_DUMPER
}
