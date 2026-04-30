#include "ObjWriterIW3.h"

#include "Game/IW3/Material/MaterialJsonDumperIW3.h"
#include "Game/IW3/Techset/TechsetDumperIW3.h"
#include "Game/IW3/XModel/XModelDumperIW3.h"
#include "Image/ImageDumperIW3.h"
#include "LightDef/LightDefDumperIW3.h"
#include "Localize/LocalizeDumperIW3.h"
#include "Maps/MapEntsDumperIW3.h"
#include "PhysPreset/PhysPresetInfoStringDumperIW3.h"
#include "RawFile/RawFileDumperIW3.h"
#include "Sound/LoadedSoundDumperIW3.h"
#include "StringTable/StringTableDumperIW3.h"

using namespace IW3;

void ObjWriter::RegisterAssetDumpers(AssetDumpingContext& context)
{
    RegisterAssetDumper(std::make_unique<phys_preset::InfoStringDumperIW3>());
    // REGISTER_DUMPER(AssetDumperXAnimParts)
    RegisterAssetDumper(std::make_unique<xmodel::DumperIW3>());
    RegisterAssetDumper(std::make_unique<material::JsonDumperIW3>());
    RegisterAssetDumper(std::make_unique<techset::DumperIW3>(
#ifdef TECHSET_DEBUG
        true
#else
        false
#endif
        ));
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
    RegisterAssetDumper(std::make_unique<light_def::DumperIW3>());
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
