#include "FontIconDumperT6.h"

#include "FontIconCsvDumperT6.h"
#include "FontIconJsonDumperT6.h"

using namespace T6;

// #define DUMP_FONT_ICON_AS_CSV 1

namespace font_icon
{
    std::unique_ptr<IAssetDumper> CreateDumperT6()
    {
#ifdef DUMP_FONT_ICON_AS_CSV
        return std::make_unique<CsvDumperT6>();
#else
        return std::make_unique<JsonDumperT6>();
#endif
    }
} // namespace font_icon
