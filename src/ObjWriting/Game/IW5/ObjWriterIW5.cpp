#include "ObjWriterIW5.h"

#include "Game/IW5/Material/MaterialJsonDumperIW5.h"
#include "Game/IW5/XModel/XModelDumperIW5.h"
#include "Image/ImageDumperIW5.h"
#include "Leaderboard/LeaderboardJsonDumperIW5.h"
#include "Localize/LocalizeDumperIW5.h"
#include "Maps/AddonMapEntsDumperIW5.h"
#include "Menu/MenuDumperIW5.h"
#include "Menu/MenuListDumperIW5.h"
#include "RawFile/RawFileDumperIW5.h"
#include "Script/ScriptDumperIW5.h"
#include "Sound/LoadedSoundDumperIW5.h"
#include "StringTable/StringTableDumperIW5.h"
#include "Weapon/AttachmentJsonDumperIW5.h"
#include "Weapon/WeaponDumperIW5.h"

using namespace IW5;

void ObjWriter::RegisterAssetDumpers(AssetDumpingContext& context)
{
    // REGISTER_DUMPER(AssetDumperPhysPreset)
    // REGISTER_DUMPER(AssetDumperPhysCollmap)
    // REGISTER_DUMPER(AssetDumperXAnimParts)
    // REGISTER_DUMPER(AssetDumperXModelSurfs)
    RegisterAssetDumper(std::make_unique<xmodel::DumperIW5>());
    RegisterAssetDumper(std::make_unique<material::JsonDumperIW5>());
    // REGISTER_DUMPER(AssetDumperMaterialPixelShader)
    // REGISTER_DUMPER(AssetDumperMaterialVertexShader)
    // REGISTER_DUMPER(AssetDumperMaterialVertexDeclaration)
    // REGISTER_DUMPER(AssetDumperMaterialTechniqueSet)
    RegisterAssetDumper(std::make_unique<image::DumperIW5>());
    // REGISTER_DUMPER(AssetDumpersnd_alias_list_t)
    // REGISTER_DUMPER(AssetDumperSndCurve)
    RegisterAssetDumper(std::make_unique<sound::LoadedSoundDumperIW5>());
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
