#include "AssetLoaderFontW3.h"
#include "Game/IW3/IW3.h"
#include "Utils/Logging/Log.h"

#include <nlohmann/json.hpp>
#include <cstring>
#include <format>

using namespace IW3;

namespace
{
    class FontLoader final : public AssetCreator<AssetFont>
    {
    public:
        FontLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(std::format("{}.json", assetName));
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* font = m_memory.Alloc<Font_s>();
            std::memset(font, 0, sizeof(Font_s));

            AssetRegistration<AssetFont> registration(assetName, font);

            if (!LoadFromJson(*file.m_stream, *font, context, registration))
            {
                con::error("Failed to load font \"{}\"", assetName);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        bool LoadFromJson(std::istream& jsonStream, Font_s& font, AssetCreationContext& context, AssetRegistration<AssetFont>& registration)
        {
            try
            {
                const auto jRoot = nlohmann::json::parse(jsonStream);

                std::string type;
                jRoot.at("_type").get_to(type);

                if (type != "font")
                {
                    con::error("Tried to load font but did not find expected type 'font'");
                    return false;
                }

                font.pixelHeight = jRoot.value("pixelHeight", 0);
                font.glyphCount = jRoot.value("glyphCount", 0);

                std::string fontName = jRoot.value("name", "");
                font.fontName = m_memory.Dup(fontName.c_str());

                if (jRoot.contains("material") && !jRoot["material"].is_null())
                {
                    std::string matName = jRoot["material"];
                    auto* matInfo = context.LoadDependency<AssetMaterial>(matName);
                    if (!matInfo)
                    {
                        con::error("Font \"{}\" missing dependency material \"{}\"", font.fontName, matName);
                        return false;
                    }
                    registration.AddDependency(matInfo);
                    font.material = matInfo->Asset();
                }

                if (jRoot.contains("glowMaterial") && !jRoot["glowMaterial"].is_null())
                {
                    std::string glowMatName = jRoot["glowMaterial"];
                    auto* glowMatInfo = context.LoadDependency<AssetMaterial>(glowMatName);
                    if (glowMatInfo)
                    {
                        registration.AddDependency(glowMatInfo);
                        font.glowMaterial = glowMatInfo->Asset();
                    }
                }

                if (font.glyphCount > 0 && jRoot.contains("glyphs"))
                {
                    const auto& jGlyphs = jRoot["glyphs"];
                    font.glyphs = m_memory.Alloc<Glyph>(font.glyphCount);

                    for (int i = 0; i < font.glyphCount; ++i)
                    {
                        const auto& jG = jGlyphs[i];
                        auto& g = font.glyphs[i];

                        g.letter = jG.value("letter", 0);

                        g.x0 = static_cast<char>(jG.value("x0", 0));
                        g.y0 = static_cast<char>(jG.value("y0", 0));
                        g.dx = static_cast<char>(jG.value("dx", 0));
                        g.pixelWidth = static_cast<char>(jG.value("width", 0));
                        g.pixelHeight = static_cast<char>(jG.value("height", 0));

                        g.s0 = jG.value("s0", 0.0f);
                        g.t0 = jG.value("t0", 0.0f);
                        g.s1 = jG.value("s1", 0.0f);
                        g.t1 = jG.value("t1", 0.0f);
                    }
                }

                return true;
            }
            catch (const nlohmann::json::exception& e)
            {
                con::error("Failed to parse json of font: {}", e.what());
            }

            return false;
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace font
{
    std::unique_ptr<AssetCreator<AssetFont>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<FontLoader>(memory, searchPath);
    }
} // namespace font
