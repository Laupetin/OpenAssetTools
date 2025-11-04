#include "ObjWriterIW5.h"

#include "Game/IW5/GameAssetPoolIW5.h"
#include "Game/IW5/Material/MaterialJsonDumperIW5.h"
#include "Game/IW5/XModel/XModelDumperIW5.h"
#include "Image/ImageDumperIW5.h"
#include "Leaderboard/LeaderboardJsonDumperIW5.h"
#include "Localize/LocalizeDumperIW5.h"
#include "Maps/AddonMapEntsDumperIW5.h"
#include "Menu/MenuDumperIW5.h"
#include "Menu/MenuListDumperIW5.h"
#include "ObjWriting.h"
#include "RawFile/RawFileDumperIW5.h"
#include "Script/ScriptDumperIW5.h"
#include "Sound/LoadedSoundDumperIW5.h"
#include "StringTable/StringTableDumperIW5.h"
#include "Weapon/AttachmentJsonDumperIW5.h"
#include "Weapon/WeaponDumperIW5.h"

using namespace IW5;

bool ObjWriter::DumpZone(AssetDumpingContext& context) const
{
#define REGISTER_DUMPER(dumperType, poolName)                                                                                                                  \
    if (assetPools->poolName && ObjWriting::ShouldHandleAssetType(dumperType::AssetType_t::EnumEntry))                                                         \
    {                                                                                                                                                          \
        dumpers.emplace_back(std::make_unique<dumperType>(*assetPools->poolName));                                                                             \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolIW5*>(context.m_zone.m_pools.get());
    std::vector<std::unique_ptr<IAssetDumper>> dumpers;

    // REGISTER_DUMPER(AssetDumperPhysPreset, m_phys_preset)
    // REGISTER_DUMPER(AssetDumperPhysCollmap, m_phys_collmap)
    // REGISTER_DUMPER(AssetDumperXAnimParts, m_xanim_parts)
    // REGISTER_DUMPER(AssetDumperXModelSurfs, m_xmodel_surfs)
    REGISTER_DUMPER(xmodel::DumperIW5, m_xmodel)
    REGISTER_DUMPER(material::JsonDumperIW5, m_material)
    // REGISTER_DUMPER(AssetDumperMaterialPixelShader, m_material_pixel_shader)
    // REGISTER_DUMPER(AssetDumperMaterialVertexShader, m_material_vertex_shader)
    // REGISTER_DUMPER(AssetDumperMaterialVertexDeclaration, m_material_vertex_decl)
    // REGISTER_DUMPER(AssetDumperMaterialTechniqueSet, m_technique_set)
    REGISTER_DUMPER(image::DumperIW5, m_image)
    // REGISTER_DUMPER(AssetDumpersnd_alias_list_t, m_sound)
    // REGISTER_DUMPER(AssetDumperSndCurve, m_sound_curve)
    REGISTER_DUMPER(sound::LoadedSoundDumperIW5, m_loaded_sound)
    // REGISTER_DUMPER(AssetDumperclipMap_t, m_clip_map)
    // REGISTER_DUMPER(AssetDumperComWorld, m_com_world)
    // REGISTER_DUMPER(AssetDumperGlassWorld, m_glass_world)
    // REGISTER_DUMPER(AssetDumperPathData, m_path_data)
    // REGISTER_DUMPER(AssetDumperVehicleTrack, m_vehicle_track)
    // REGISTER_DUMPER(AssetDumperMapEnts, m_map_ents)
    // REGISTER_DUMPER(AssetDumperFxWorld, m_fx_world)
    // REGISTER_DUMPER(AssetDumperGfxWorld, m_gfx_world)
    // REGISTER_DUMPER(AssetDumperGfxLightDef, m_gfx_light_def)
    // REGISTER_DUMPER(AssetDumperFont_s, m_font)
    REGISTER_DUMPER(menu::MenuListDumperIW5, m_menu_list)
    REGISTER_DUMPER(menu::MenuDumperIW5, m_menu_def)
    REGISTER_DUMPER(localize::DumperIW5, m_localize)
    REGISTER_DUMPER(attachment::JsonDumperIW5, m_attachment)
    REGISTER_DUMPER(weapon::DumperIW5, m_weapon)
    // REGISTER_DUMPER(AssetDumperFxEffectDef, m_fx)
    // REGISTER_DUMPER(AssetDumperFxImpactTable, m_fx_impact_table)
    // REGISTER_DUMPER(AssetDumperSurfaceFxTable, m_surface_fx_table)
    REGISTER_DUMPER(raw_file::DumperIW5, m_raw_file)
    REGISTER_DUMPER(script::DumperIW5, m_script_file)
    REGISTER_DUMPER(string_table::DumperIW5, m_string_table)
    REGISTER_DUMPER(leaderboard::JsonDumperIW5, m_leaderboard)
    // REGISTER_DUMPER(AssetDumperStructuredDataDefSet, m_structed_data_def_set)
    // REGISTER_DUMPER(AssetDumperTracerDef, m_tracer)
    // REGISTER_DUMPER(AssetDumperVehicleDef, m_vehicle)
    REGISTER_DUMPER(addon_map_ents::DumperIW5, m_addon_map_ents)

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
