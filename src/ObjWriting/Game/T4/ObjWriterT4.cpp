#include "ObjWriterT4.h"

#include "Game/T4/Image/ImageDumperT4.h"
#include "Game/T4/XAnim/XAnimDumperT4.h"
#include "Game/T4/XModel/XModelDumperT4.h"
#include "Localize/LocalizeDumperT4.h"
#include "Maps/MapEntsDumperT4.h"
#include "RawFile/RawFileDumperT4.h"
#include "Sound/LoadedSoundDumperT4.h"
#include "StringTable/StringTableDumperT4.h"

using namespace T4;

void ObjWriter::RegisterAssetDumpers(AssetDumpingContext& context)
{
    RegisterAssetDumper(std::make_unique<xanim::DumperT4>());
    RegisterAssetDumper(std::make_unique<xmodel::DumperT4>());
    RegisterAssetDumper(std::make_unique<image::DumperT4>());
    RegisterAssetDumper(std::make_unique<sound::LoadedSoundDumperT4>());
    RegisterAssetDumper(std::make_unique<map_ents::DumperT4>());
    RegisterAssetDumper(std::make_unique<localize::DumperT4>());
    RegisterAssetDumper(std::make_unique<raw_file::DumperT4>());
    RegisterAssetDumper(std::make_unique<string_table::DumperT4>());
}
