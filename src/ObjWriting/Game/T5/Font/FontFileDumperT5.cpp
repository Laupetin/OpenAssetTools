#include "FontFileDumperT5.h"
#include <nlohmann/json.hpp>

using ordered_json = nlohmann::ordered_json;
using namespace nlohmann;
using namespace T5;

namespace font
{
    void DumperT5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetFont::Type>& asset)
    {
        const auto* fontFile = asset.Asset();
        const auto assetFile = context.OpenAssetFile(std::format("{}.json", asset.m_name));

        if (!assetFile)
            return;

        ordered_json jRoot;
        jRoot["$schema"] = "http://openassettools.dev/schema/font.v1.json";
        jRoot["_type"] = "font";
        jRoot["_game"] = "t5";

        jRoot["name"] = fontFile->fontName ? fontFile->fontName : "";
        jRoot["pixelHeight"] = fontFile->pixelHeight;
        jRoot["glyphCount"] = fontFile->glyphCount;

        if (fontFile->material && fontFile->material->info.name)
        {
            jRoot["material"] = fontFile->material->info.name;
        }
        else
        {
            jRoot["material"] = nullptr;
        }

        if (fontFile->glowMaterial && fontFile->glowMaterial->info.name)
        {
            jRoot["glowMaterial"] = fontFile->glowMaterial->info.name;
        }

        jRoot["glyphs"] = json::array();

        if (fontFile->glyphs && fontFile->glyphCount > 0)
        {
            for (int i = 0; i < fontFile->glyphCount; ++i)
            {
                const auto& g = fontFile->glyphs[i];

                ordered_json glyphJson;
                glyphJson["letter"] = g.letter;

                glyphJson["x0"] = static_cast<int>(g.x0);
                glyphJson["y0"] = static_cast<int>(g.y0);
                glyphJson["dx"] = static_cast<int>(g.dx);
                glyphJson["width"] = static_cast<int>(g.pixelWidth);
                glyphJson["height"] = static_cast<int>(g.pixelHeight);

                glyphJson["s0"] = g.s0;
                glyphJson["t0"] = g.t0;
                glyphJson["s1"] = g.s1;
                glyphJson["t1"] = g.t1;

                jRoot["glyphs"].push_back(glyphJson);
            }
        }

        auto& stream = *assetFile;
        stream << jRoot.dump(4) << std::endl;
    }
} // namespace font
