#include "FontDumperIW3.h"

#include "FontWriterIW3.h"
#include "ObjWriting.h"

#include <filesystem>
#include <string>
#include <Font/FontDumpingZoneState.h>

using namespace IW3;

namespace
{
    std::string GetPathForFont(font::FontDumpingZoneState* zoneState, const XAssetInfo<Font_s>& asset)
    {
        const auto menuDumpingState = zoneState->m_font_dumping_state_map.find(asset.Asset());

        if (menuDumpingState == zoneState->m_font_dumping_state_map.end())
            return "fonts/" + std::string(asset.Asset()->fontName);

        return menuDumpingState->second.m_path;
    }
} // namespace

namespace font
{
    void FontDumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetFont::Type>& asset)
    {
        const auto* font = asset.Asset();
        auto* zoneState = context.GetZoneAssetDumperState<FontDumpingZoneState>();

        const auto fontFilePath = GetPathForFont(zoneState, asset);
        const auto assetFile = context.OpenAssetFile(fontFilePath);

        if (!assetFile)
            return;

        auto fontWriter = CreateFontWriterIW3(*assetFile);

        fontWriter->WriteFont(*font);
    }
} // namespace font
