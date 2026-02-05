#include "ObjWriterIW3.h"

#include "Game/IW3/Material/MaterialJsonDumperIW3.h"
#include "Game/IW3/XModel/XModelDumperIW3.h"
#include "Image/ImageDumperIW3.h"
#include "Localize/LocalizeDumperIW3.h"
#include "Maps/MapEntsDumperIW3.h"
#include "RawFile/RawFileDumperIW3.h"
#include "Sound/LoadedSoundDumperIW3.h"
#include "StringTable/StringTableDumperIW3.h"

using namespace IW3;

void ObjWriter::RegisterAssetDumpers(AssetDumpingContext& context)
{
    // REGISTER_DUMPER(AssetDumperPhysPreset)
    // REGISTER_DUMPER(AssetDumperXAnimParts)
    RegisterAssetDumper(std::make_unique<xmodel::DumperIW3>());
    RegisterAssetDumper(std::make_unique<material::JsonDumperIW3>());
    // REGISTER_DUMPER(AssetDumperMaterialTechniqueSet)
    RegisterAssetDumper(std::make_unique<image::DumperIW3>());
    // REGISTER_DUMPER(AssetDumpersnd_alias_list_t)
    // REGISTER_DUMPER(AssetDumperSndCurve)
    RegisterAssetDumper(std::make_unique<sound::LoadedSoundDumperIW3>());
    // REGISTER_DUMPER(AssetDumperClipMap)
    // REGISTER_DUMPER(AssetDumperComWorld)
    // REGISTER_DUMPER(AssetDumperGameWorldSp)
    // REGISTER_DUMPER(AssetDumperGameWorldMp)
    RegisterAssetDumper(std::make_unique<map_ents::DumperIW3>());
    // REGISTER_DUMPER(AssetDumperGfxWorld)
    // REGISTER_DUMPER(AssetDumperGfxLightDef)
    // REGISTER_DUMPER(AssetDumperFont_s)
    // REGISTER_DUMPER(AssetDumperMenuList)
    // REGISTER_DUMPER(AssetDumpermenuDef_t)
    RegisterAssetDumper(std::make_unique<localize::DumperIW3>());
    // REGISTER_DUMPER(AssetDumperWeapon)
    // REGISTER_DUMPER(AssetDumperSndDriverGlobals)
    // REGISTER_DUMPER(AssetDumperFxEffectDef)
    // REGISTER_DUMPER(AssetDumperFxImpactTable)
    RegisterAssetDumper(std::make_unique<raw_file::DumperIW3>());
    RegisterAssetDumper(std::make_unique<string_table::DumperIW3>());
}
