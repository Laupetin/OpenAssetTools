#include "ObjWriterCODO.h"

#include "Localize/LocalizeDumperCODO.h"
#include "RawFile/RawFileDumperCODO.h"
#include "StringTable/StringTableDumperCODO.h"

using namespace CODO;

void ObjWriter::RegisterAssetDumpers(AssetDumpingContext& context)
{
    RegisterAssetDumper(std::make_unique<localize::DumperCODO>());
    RegisterAssetDumper(std::make_unique<raw_file::DumperCODO>());
    RegisterAssetDumper(std::make_unique<string_table::DumperCODO>());
}
