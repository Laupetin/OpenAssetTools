#include "JsonLoaderFontIconT6.h"

#include "FontIcon/FontIconCommon.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/FontIcon/JsonFontIconT6.h"
#include "Game/T6/T6.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace T6;

namespace
{
    class JsonFontIconLoader final : public AssetCreator<AssetFontIcon>
    {
    public:
        JsonFontIconLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(font_icon::GetJsonFileNameForAssetName(assetName));
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* fontIcon = m_memory.Alloc<FontIcon>();
            fontIcon->name = m_memory.Dup(assetName.c_str());
            AssetRegistration<AssetFontIcon> registration(assetName, fontIcon);

            try
            {
                const auto jRoot = json::parse(*file.m_stream);
                std::string type;
                unsigned version;

                jRoot.at("_type").get_to(type);
                jRoot.at("_version").get_to(version);

                if (type != "font-icon" || version != 1u)
                {
                    std::cerr << std::format("Tried to load font icon \"{}\" but did not find expected type font-icon of version 1\n", assetName);
                    return AssetCreationResult::Failure();
                }

                const auto jFontIcon = jRoot.get<JsonFontIcon>();
                if (CreateFontIconFromJson(jFontIcon, *fontIcon, registration, context))
                    return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
            }
            catch (const json::exception& e)
            {
                std::cerr << std::format("Failed to parse json of font icon: {}\n", e.what());
            }

            return AssetCreationResult::Failure();
        }

    private:
        bool CreateFontIconFromJson(const JsonFontIcon& jFontIcon,
                                    FontIcon& fontIcon,
                                    AssetRegistration<AssetFontIcon>& registration,
                                    AssetCreationContext& context) const
        {
            std::vector<FontIconAlias> aliases;

            fontIcon.numEntries = static_cast<unsigned>(jFontIcon.entries.size());
            fontIcon.fontIconEntry = m_memory.Alloc<FontIconEntry>(fontIcon.numEntries);

            for (auto entryIndex = 0u; entryIndex < fontIcon.numEntries; entryIndex++)
            {
                if (!CreateFontIconEntryFromJson(jFontIcon.entries[entryIndex], fontIcon.fontIconEntry[entryIndex], aliases, registration, context))
                    return false;
            }

            std::sort(fontIcon.fontIconEntry,
                      &fontIcon.fontIconEntry[fontIcon.numEntries],
                      [](const FontIconEntry& e0, const FontIconEntry& e1) -> bool
                      {
                          return e0.fontIconName.hash < e1.fontIconName.hash;
                      });

            if (!aliases.empty())
            {
                fontIcon.numAliasEntries = static_cast<unsigned>(aliases.size());
                fontIcon.fontIconAlias = m_memory.Alloc<FontIconAlias>(fontIcon.numAliasEntries);
                std::memcpy(fontIcon.fontIconAlias, aliases.data(), sizeof(FontIconAlias) * fontIcon.numAliasEntries);

                std::sort(fontIcon.fontIconAlias,
                          &fontIcon.fontIconAlias[fontIcon.numAliasEntries],
                          [](const FontIconAlias& a0, const FontIconAlias& a1) -> bool
                          {
                              return a0.aliasHash < a1.aliasHash;
                          });
            }

            return true;
        }

        bool CreateFontIconEntryFromJson(const JsonFontIconEntry& jFontIconEntry,
                                         FontIconEntry& fontIconEntry,
                                         std::vector<FontIconAlias>& aliases,
                                         AssetRegistration<AssetFontIcon>& registration,
                                         AssetCreationContext& context) const
        {
            fontIconEntry.fontIconName.string = m_memory.Dup(jFontIconEntry.name.c_str());
            fontIconEntry.fontIconName.hash = Common::Com_HashString(jFontIconEntry.name.c_str());

            auto* materialDependency = context.LoadDependency<AssetMaterial>(jFontIconEntry.material);
            if (materialDependency == nullptr)
            {
                std::cerr << std::format("Failed to load material \"{}\" for font icon entry \"{}\"\n", jFontIconEntry.material, jFontIconEntry.name);
                return false;
            }
            registration.AddDependency(materialDependency);
            fontIconEntry.fontIconMaterialHandle = materialDependency->Asset();

            fontIconEntry.fontIconSize = static_cast<int>(jFontIconEntry.size);
            fontIconEntry.xScale = jFontIconEntry.scale.has_value() ? jFontIconEntry.scale->x : 0;
            fontIconEntry.yScale = jFontIconEntry.scale.has_value() ? jFontIconEntry.scale->y : 0;

            for (const auto& alias : jFontIconEntry.aliases)
                aliases.emplace_back(Common::Com_HashString(alias.c_str()), fontIconEntry.fontIconName.hash);

            if (jFontIconEntry.aliasHashes.has_value())
            {
                for (const auto aliasHash : *jFontIconEntry.aliasHashes)
                    aliases.emplace_back(aliasHash, fontIconEntry.fontIconName.hash);
            }

            return true;
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace font_icon
{
    std::unique_ptr<AssetCreator<AssetFontIcon>> CreateJsonLoaderT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<JsonFontIconLoader>(memory, searchPath);
    }
} // namespace font_icon
