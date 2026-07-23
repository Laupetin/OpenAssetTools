#include "ObjWriterQOS.h"

#include "Game/QOS/Font/FontDumperQOS.h"
#include "Game/QOS/Image/ImageDumperQOS.h"
#include "Game/QOS/XModel/XModelDumperQOS.h"
#include "Localize/LocalizeDumperQOS.h"
#include "Maps/MapEntsDumperQOS.h"
#include "RawFile/RawFileDumperQOS.h"
#include "StringTable/StringTableDumperQOS.h"

using namespace QOS;

void ObjWriter::RegisterAssetDumpers(AssetDumpingContext& context)
{
    RegisterAssetDumper(std::make_unique<font::JsonDumperQOS>());
    RegisterAssetDumper(std::make_unique<image::DumperQOS>());
    RegisterAssetDumper(std::make_unique<localize::DumperQOS>());
    RegisterAssetDumper(std::make_unique<map_ents::DumperQOS>());
    RegisterAssetDumper(std::make_unique<raw_file::DumperQOS>());
    RegisterAssetDumper(std::make_unique<string_table::DumperQOS>());
    RegisterAssetDumper(std::make_unique<xmodel::DumperQOS>());
}
