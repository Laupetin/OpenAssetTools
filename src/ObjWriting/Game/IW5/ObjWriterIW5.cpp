#include "ObjWriterIW5.h"

#include "Game/IW5/Material/MaterialJsonDumperIW5.h"
#include "Game/IW5/Techset/PixelShaderDumperIW5.h"
#include "Game/IW5/Techset/TechsetDumperIW5.h"
#include "Game/IW5/Techset/VertexShaderDumperIW5.h"
#include "Game/IW5/XModel/XModelDumperIW5.h"
#include "Image/ImageDumperIW5.h"
#include "Leaderboard/LeaderboardJsonDumperIW5.h"
#include "LightDef/LightDefDumperIW5.h"
#include "Localize/LocalizeDumperIW5.h"
#include "Maps/AddonMapEntsDumperIW5.h"
#include "Menu/MenuDumperIW5.h"
#include "Menu/MenuListDumperIW5.h"
#include "PhysPreset/PhysPresetInfoStringDumperIW5.h"
#include "RawFile/RawFileDumperIW5.h"
#include "Script/ScriptDumperIW5.h"
#include "Sound/LoadedSoundDumperIW5.h"
#include "Sound/SndCurveDumperIW5.h"
#include "StringTable/StringTableDumperIW5.h"
#include "Weapon/AttachmentJsonDumperIW5.h"
#include "Weapon/WeaponDumperIW5.h"

using namespace IW5;

void ObjWriter::RegisterAssetDumpers(AssetDumpingContext& context)
{
    RegisterAssetDumper(std::make_unique<phys_preset::InfoStringDumperIW5>());
    // REGISTER_DUMPER(AssetDumperPhysCollmap)
    // REGISTER_DUMPER(AssetDumperXAnimParts)
    // REGISTER_DUMPER(AssetDumperXModelSurfs)
    RegisterAssetDumper(std::make_unique<xmodel::DumperIW5>());
    RegisterAssetDumper(std::make_unique<material::JsonDumperIW5>());
    RegisterAssetDumper(std::make_unique<techset::PixelShaderDumperIW5>());
    RegisterAssetDumper(std::make_unique<techset::VertexShaderDumperIW5>());
    RegisterAssetDumper(std::make_unique<techset::DumperIW5>(
#ifdef TECHSET_DEBUG
        true
#else
        false
#endif
        ));
    RegisterAssetDumper(std::make_unique<image::DumperIW5>());
    // REGISTER_DUMPER(AssetDumpersnd_alias_list_t)
    RegisterAssetDumper(std::make_unique<sound_curve::DumperIW5>());
    RegisterAssetDumper(std::make_unique<sound::LoadedSoundDumperIW5>());
    // REGISTER_DUMPER(AssetDumperclipMap_t)
    // REGISTER_DUMPER(AssetDumperComWorld)
    // REGISTER_DUMPER(AssetDumperGlassWorld)
    // REGISTER_DUMPER(AssetDumperPathData)
    // REGISTER_DUMPER(AssetDumperVehicleTrack)
    // REGISTER_DUMPER(AssetDumperMapEnts)
    // REGISTER_DUMPER(AssetDumperFxWorld)
    // REGISTER_DUMPER(AssetDumperGfxWorld)
    RegisterAssetDumper(std::make_unique<light_def::DumperIW5>());
    // REGISTER_DUMPER(AssetDumperFont_s)
    RegisterAssetDumper(std::make_unique<menu::MenuListDumperIW5>());
    RegisterAssetDumper(std::make_unique<menu::MenuDumperIW5>());
    RegisterAssetDumper(std::make_unique<localize::DumperIW5>());
    RegisterAssetDumper(std::make_unique<attachment::JsonDumperIW5>());
    RegisterAssetDumper(std::make_unique<weapon::DumperIW5>());
    // REGISTER_DUMPER(AssetDumperFxEffectDef)
    // REGISTER_DUMPER(AssetDumperFxImpactTable)
    // REGISTER_DUMPER(AssetDumperSurfaceFxTable)
    RegisterAssetDumper(std::make_unique<raw_file::DumperIW5>());
    RegisterAssetDumper(std::make_unique<script::DumperIW5>());
    RegisterAssetDumper(std::make_unique<string_table::DumperIW5>());
    RegisterAssetDumper(std::make_unique<leaderboard::JsonDumperIW5>());
    // REGISTER_DUMPER(AssetDumperStructuredDataDefSet)
    // REGISTER_DUMPER(AssetDumperTracerDef)
    // REGISTER_DUMPER(AssetDumperVehicleDef)
    RegisterAssetDumper(std::make_unique<addon_map_ents::DumperIW5>());
}
