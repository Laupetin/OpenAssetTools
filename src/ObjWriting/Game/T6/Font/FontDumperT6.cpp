#include "FontDumperT6.h"

#include "Game/T6/Font/JsonFontT6.h"
#include "Localize/LocalizeCommon.h"

#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace T6;

namespace
{
    void CreateJsonGlyph(JsonGlyph& jGlyph, const Glyph& glyph)
    {
        const auto c = glyph.letter;

        if (c < 0x80)
            jGlyph.letter = std::string(1, static_cast<char>(c));
        else if (c < 0x800)
        {
            jGlyph.letter += static_cast<char>(0xC0 | (c >> 6));
            jGlyph.letter += static_cast<char>(0x80 | (c & 0x3F));
        }
        else
        {
            jGlyph.letter += static_cast<char>(0xE0 | (c >> 12));
            jGlyph.letter += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
            jGlyph.letter += static_cast<char>(0x80 | (c & 0x3F));
        }

        jGlyph.x0 = glyph.x0;
        jGlyph.y0 = glyph.y0;
        jGlyph.dx = glyph.dx;
        jGlyph.pixelWidth = glyph.pixelWidth;
        jGlyph.pixelHeight = glyph.pixelHeight;
        jGlyph.s0 = glyph.s0;
        jGlyph.t0 = glyph.t0;
        jGlyph.s1 = glyph.s1;
        jGlyph.t1 = glyph.t1;
    }

    void CreateJsonKerningPair(JsonKerningPair& jPair, const KerningPairs& pair)
    {
        jPair.wFirst = pair.wFirst;
        jPair.wSecond = pair.wSecond;
        jPair.iKernAmount = pair.iKernAmount;
    }

    void CreateJsonFontFile(JsonFont& jFont, const Font_s& font)
    {
        jFont.pixelHeight = font.pixelHeight;
        jFont.isScalingAllowed = font.isScalingAllowed != 0;

        if (font.material && font.material->info.name)
            jFont.material = font.material->info.name;

        if (font.glowMaterial && font.glowMaterial->info.name)
            jFont.glowMaterial = font.glowMaterial->info.name;

        if (font.glyphs && font.glyphCount > 0)
        {
            jFont.glyphs.resize(font.glyphCount);
            for (auto i = 0; i < font.glyphCount; i++)
                CreateJsonGlyph(jFont.glyphs[i], font.glyphs[i]);
        }

        if (font.kerningPairs && font.kerningPairsCount > 0)
        {
            jFont.kerningPairs.resize(font.kerningPairsCount);
            for (auto i = 0; i < font.kerningPairsCount; i++)
                CreateJsonKerningPair(jFont.kerningPairs[i], font.kerningPairs[i]);
        }
    }

    void DumpFont(std::ostream& stream, const Font_s& font)
    {
        JsonFont jFont;
        CreateJsonFontFile(jFont, font);

        ordered_json jRoot;
        jRoot["_type"] = "font";
        jRoot["_version"] = 1;
        jRoot["_game"] = "t6";
        jRoot["pixelHeight"] = jFont.pixelHeight;
        jRoot["isScalingAllowed"] = jFont.isScalingAllowed;
        if (jFont.material)
            jRoot["material"] = *jFont.material;
        if (jFont.glowMaterial)
            jRoot["glowMaterial"] = *jFont.glowMaterial;
        jRoot["glyphs"] = ordered_json(jFont.glyphs);
        jRoot["kerningPairs"] = ordered_json(jFont.kerningPairs);

        stream << std::setw(4) << jRoot << "\n";
    }
} // namespace

namespace font
{
    void DumperT6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetFont::Type>& asset)
    {
        const auto language = LocalizeCommon::GetNameOfLanguage(context.m_zone.m_language);
        const auto assetFile = context.OpenAssetFile(std::format("{}/{}.json", language, asset.m_name));

        if (!assetFile)
            return;

        DumpFont(*assetFile, *asset.Asset());
    }
} // namespace font
