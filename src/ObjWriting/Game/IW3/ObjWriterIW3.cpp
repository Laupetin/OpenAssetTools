#include "ObjWriterIW3.h"

#include "Game/IW3/GameAssetPoolIW3.h"
#include "Game/IW3/Material/MaterialJsonDumperIW3.h"
#include "Game/IW3/XModel/XModelDumperIW3.h"
#include "Image/ImageDumperIW3.h"
#include "Localize/LocalizeDumperIW3.h"
#include "Maps/MapEntsDumperIW3.h"
#include "ObjWriting.h"
#include "RawFile/RawFileDumperIW3.h"
#include "Sound/LoadedSoundDumperIW3.h"
#include "StringTable/StringTableDumperIW3.h"

using namespace IW3;

bool ObjWriter::DumpZone(AssetDumpingContext& context) const
{
#define REGISTER_DUMPER(dumperType, poolName)                                                                                                                  \
    if (assetPools->poolName && ObjWriting::ShouldHandleAssetType(dumperType::AssetType_t::EnumEntry))                                                         \
    {                                                                                                                                                          \
        dumpers.emplace_back(std::make_unique<dumperType>(*assetPools->poolName));                                                                             \
    }

    const auto* assetPools = dynamic_cast<GameAssetPoolIW3*>(context.m_zone.m_pools.get());
    std::vector<std::unique_ptr<IAssetDumper>> dumpers;

    // REGISTER_DUMPER(AssetDumperPhysPreset, m_phys_preset)
    // REGISTER_DUMPER(AssetDumperXAnimParts, m_xanim_parts)
    REGISTER_DUMPER(xmodel::DumperIW3, m_xmodel)
    REGISTER_DUMPER(material::JsonDumperIW3, m_material)
    // REGISTER_DUMPER(AssetDumperMaterialTechniqueSet, m_technique_set)
    REGISTER_DUMPER(image::DumperIW3, m_image)
    // REGISTER_DUMPER(AssetDumpersnd_alias_list_t, m_sound)
    // REGISTER_DUMPER(AssetDumperSndCurve, m_sound_curve)
    REGISTER_DUMPER(sound::LoadedSoundDumperIW3, m_loaded_sound)
    // REGISTER_DUMPER(AssetDumperClipMap, m_clip_map)
    // REGISTER_DUMPER(AssetDumperComWorld, m_com_world)
    // REGISTER_DUMPER(AssetDumperGameWorldSp, m_game_world_sp)
    // REGISTER_DUMPER(AssetDumperGameWorldMp, m_game_world_mp)
    REGISTER_DUMPER(map_ents::DumperIW3, m_map_ents)
    // REGISTER_DUMPER(AssetDumperGfxWorld, m_gfx_world)
    // REGISTER_DUMPER(AssetDumperGfxLightDef, m_gfx_light_def)
    // REGISTER_DUMPER(AssetDumperFont_s, m_font)
    // REGISTER_DUMPER(AssetDumperMenuList, m_menu_list)
    // REGISTER_DUMPER(AssetDumpermenuDef_t, m_menu_def)
    REGISTER_DUMPER(localize::DumperIW3, m_localize)
    // REGISTER_DUMPER(AssetDumperWeapon, m_weapon)
    // REGISTER_DUMPER(AssetDumperSndDriverGlobals, m_snd_driver_globals)
    // REGISTER_DUMPER(AssetDumperFxEffectDef, m_fx)
    // REGISTER_DUMPER(AssetDumperFxImpactTable, m_fx_impact_table)
    REGISTER_DUMPER(raw_file::DumperIW3, m_raw_file)
    REGISTER_DUMPER(string_table::DumperIW3, m_string_table)

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
