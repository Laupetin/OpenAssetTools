#include "DumperFontIconT6.h"

#include "DumperFontIconCsvT6.h"
#include "DumperFontIconJsonT6.h"

using namespace T6;

// #define DUMP_FONT_ICON_AS_CSV 1

bool AssetDumperFontIcon::ShouldDump(XAssetInfo<FontIcon>* asset)
{
    return true;
}

void AssetDumperFontIcon::DumpAsset(AssetDumpingContext& context, XAssetInfo<FontIcon>* asset)
{
#ifdef DUMP_FONT_ICON_AS_CSV
    DumpFontIconAsCsv(context, *asset->Asset());
#else
    DumpFontIconAsJson(context, *asset->Asset());
#endif
}
