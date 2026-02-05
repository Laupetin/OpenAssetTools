#include "ObjWriterIW4.h"

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

void ObjWriter::RegisterAssetDumpers(AssetDumpingContext& context)
{
    RegisterAssetDumper(std::make_unique<phys_preset::InfoStringDumperIW4>());
    RegisterAssetDumper(std::make_unique<phys_collmap::DumperIW4>());
    // REGISTER_DUMPER(AssetDumperXAnimParts)
    RegisterAssetDumper(std::make_unique<xmodel::DumperIW4>());
    RegisterAssetDumper(std::make_unique<material::JsonDumperIW4>());
#ifdef EXPERIMENTAL_MATERIAL_COMPILATION
    RegisterAssetDumper(std::make_unique<material::DecompilingGdtDumperIW4>());
#endif
    RegisterAssetDumper(std::make_unique<shader::PixelShaderDumperIW4>());
    RegisterAssetDumper(std::make_unique<shader::VertexShaderDumperIW4>());
    RegisterAssetDumper(std::make_unique<techset::DumperIW4>());
    RegisterAssetDumper(std::make_unique<image::DumperIW4>());
    // REGISTER_DUMPER(AssetDumpersnd_alias_list_t)
    RegisterAssetDumper(std::make_unique<sound_curve::DumperIW4>());
    RegisterAssetDumper(std::make_unique<sound::LoadedSoundDumperIW4>());
    // REGISTER_DUMPER(AssetDumperClipMap)
    // REGISTER_DUMPER(AssetDumperComWorld)
    // REGISTER_DUMPER(AssetDumperGameWorldSp)
    // REGISTER_DUMPER(AssetDumperGameWorldMp)
    // REGISTER_DUMPER(AssetDumperMapEnts)
    // REGISTER_DUMPER(AssetDumperFxWorld)
    // REGISTER_DUMPER(AssetDumperGfxWorld)
    RegisterAssetDumper(std::make_unique<light_def::DumperIW4>());
    // REGISTER_DUMPER(AssetDumperFont_s)
    RegisterAssetDumper(std::make_unique<menu::MenuListDumperIW4>());
    RegisterAssetDumper(std::make_unique<menu::MenuDumperIW4>());
    RegisterAssetDumper(std::make_unique<localize::DumperIW4>());
    RegisterAssetDumper(std::make_unique<weapon::DumperIW4>());
    // REGISTER_DUMPER(AssetDumperSndDriverGlobals)
    // REGISTER_DUMPER(AssetDumperFxEffectDef)
    // REGISTER_DUMPER(AssetDumperFxImpactTable)
    RegisterAssetDumper(std::make_unique<raw_file::DumperIW4>());
    RegisterAssetDumper(std::make_unique<string_table::DumperIW4>());
    RegisterAssetDumper(std::make_unique<leaderboard::JsonDumperIW4>());
    RegisterAssetDumper(std::make_unique<structured_data_def::DumperIW4>());
    RegisterAssetDumper(std::make_unique<tracer::DumperIW4>());
    RegisterAssetDumper(std::make_unique<vehicle::DumperIW4>());
    RegisterAssetDumper(std::make_unique<addon_map_ents::DumperIW4>());
}
