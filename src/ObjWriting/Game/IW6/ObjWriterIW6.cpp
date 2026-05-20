#include "ObjWriterIW6.h"

#include "Game/IW6/Material/MaterialJsonDumperIW6.h"
#include "Game/IW6/Techset/PixelShaderDumperIW6.h"
#include "Game/IW6/Techset/TechsetDumperIW6.h"
#include "Game/IW6/Techset/VertexShaderDumperIW6.h"
#include "Game/IW6/XModel/XModelDumperIW6.h"
#include "Image/ImageDumperIW6.h"
#include "Leaderboard/LeaderboardJsonDumperIW6.h"
#include "Localize/LocalizeDumperIW6.h"
#include "Maps/AddonMapEntsDumperIW6.h"
#include "Menu/MenuDumperIW6.h"
#include "Menu/MenuListDumperIW6.h"
#include "PhysPreset/PhysPresetInfoStringDumperIW6.h"
#include "RawFile/RawFileDumperIW6.h"
#include "Script/ScriptDumperIW6.h"
#include "Sound/LoadedSoundDumperIW6.h"
#include "Sound/SndCurveDumperIW6.h"
#include "StringTable/StringTableDumperIW6.h"
#include "Weapon/AttachmentJsonDumperIW6.h"
#include "Weapon/WeaponDumperIW6.h"

using namespace IW6;

void ObjWriter::RegisterAssetDumpers(AssetDumpingContext& context)
{
    RegisterAssetDumper(std::make_unique<phys_preset::InfoStringDumperIW6>());
    // REGISTER_DUMPER(AssetDumperPhysCollmap)
    // REGISTER_DUMPER(AssetDumperXAnimParts)
    // REGISTER_DUMPER(AssetDumperXModelSurfs)
    RegisterAssetDumper(std::make_unique<xmodel::DumperIW6>());
    RegisterAssetDumper(std::make_unique<material::JsonDumperIW6>());
    RegisterAssetDumper(std::make_unique<techset::PixelShaderDumperIW6>());
    RegisterAssetDumper(std::make_unique<techset::VertexShaderDumperIW6>());
    RegisterAssetDumper(std::make_unique<techset::DumperIW6>(
#ifdef TECHSET_DEBUG
        true
#else
        false
#endif
        ));
    RegisterAssetDumper(std::make_unique<image::DumperIW6>());
    // REGISTER_DUMPER(AssetDumpersnd_alias_list_t)
    RegisterAssetDumper(std::make_unique<sound_curve::DumperIW6>());
    RegisterAssetDumper(std::make_unique<sound::LoadedSoundDumperIW6>());
    // REGISTER_DUMPER(AssetDumperclipMap_t)
    // REGISTER_DUMPER(AssetDumperComWorld)
    // REGISTER_DUMPER(AssetDumperGlassWorld)
    // REGISTER_DUMPER(AssetDumperPathData)
    // REGISTER_DUMPER(AssetDumperVehicleTrack)
    // REGISTER_DUMPER(AssetDumperMapEnts)
    // REGISTER_DUMPER(AssetDumperFxWorld)
    // REGISTER_DUMPER(AssetDumperGfxWorld)
    // REGISTER_DUMPER(AssetDumperGfxLightDef)
    // REGISTER_DUMPER(AssetDumperFont_s)
    RegisterAssetDumper(std::make_unique<menu::MenuListDumperIW6>());
    RegisterAssetDumper(std::make_unique<menu::MenuDumperIW6>());
    RegisterAssetDumper(std::make_unique<localize::DumperIW6>());
    RegisterAssetDumper(std::make_unique<attachment::JsonDumperIW6>());
    RegisterAssetDumper(std::make_unique<weapon::DumperIW6>());
    // REGISTER_DUMPER(AssetDumperFxEffectDef)
    // REGISTER_DUMPER(AssetDumperFxImpactTable)
    // REGISTER_DUMPER(AssetDumperSurfaceFxTable)
    RegisterAssetDumper(std::make_unique<raw_file::DumperIW6>());
    RegisterAssetDumper(std::make_unique<script::DumperIW6>());
    RegisterAssetDumper(std::make_unique<string_table::DumperIW6>());
    RegisterAssetDumper(std::make_unique<leaderboard::JsonDumperIW6>());
    // REGISTER_DUMPER(AssetDumperStructuredDataDefSet)
    // REGISTER_DUMPER(AssetDumperTracerDef)
    // REGISTER_DUMPER(AssetDumperVehicleDef)
    RegisterAssetDumper(std::make_unique<addon_map_ents::DumperIW6>());
}
