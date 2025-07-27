#include "FontIconDumperT6.h"

#include "FontIconCsvDumperT6.h"
#include "FontIconJsonDumperT6.h"

using namespace T6;

// #define DUMP_FONT_ICON_AS_CSV 1

namespace T6::font_icon
{
    std::unique_ptr<IAssetDumper<FontIcon>> CreateDumper()
    {
#ifdef DUMP_FONT_ICON_AS_CSV
        return std::make_unique<CsvDumper>();
#else
        return std::make_unique<JsonDumper>();
#endif
    }
} // namespace T6::font_icon
