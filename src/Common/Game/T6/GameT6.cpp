#include "GameT6.h"

#include "T6.h"

#include <algorithm>

using namespace T6;

namespace
{
    constexpr const char* ASSET_TYPE_NAMES[]{
        "xmodelpieces",
        "physpreset",
        "physconstraints",
        "destructibledef",
        "xanim",
        "xmodel",
        "material",
        "techniqueset",
        "image",
        "soundbank",
        "soundpatch",
        "clipmap_unused",
        "clipmap",
        "comworld",
        "gameworldsp",
        "gameworldmp",
        "mapents",
        "gfxworld",
        "gfxlightdef",
        "uimap",
        "font",
        "fonticon",
        "menulist",
        "menu",
        "localize",
        "weapon",
        "weapondef",
        "weaponvariant",
        "weaponfull",
        "attachment",
        "attachmentunique",
        "camo",
        "snddriverglobals",
        "fx",
        "fximpacttable",
        "aitype",
        "mptype",
        "mpbody",
        "mphead",
        "character",
        "xmodelalias",
        "rawfile",
        "stringtable",
        "leaderboard",
        "xglobals",
        "ddl",
        "glasses",
        "emblemset",
        "script",
        "keyvaluepairs",
        "vehicle",
        "memoryblock",
        "addonmapents",
        "tracer",
        "skinnedverts",
        "qdb",
        "slug",
        "footsteptable",
        "footstepfxtable",
        "zbarrier",
    };
    static_assert(std::extent_v<decltype(ASSET_TYPE_NAMES)> == ASSET_TYPE_COUNT);

    constexpr const char* SUB_ASSET_TYPE_NAMES[]{
        "technique",
        "vertexshader",
        "pixelshader",
    };
    static_assert(std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)> == SUB_ASSET_TYPE_COUNT);
} // namespace

namespace T6
{
    GameId Game::GetId() const
    {
        return GameId::T6;
    }

    const std::string& Game::GetFullName() const
    {
        static std::string fullName = "Call Of Duty: Black Ops II";
        return fullName;
    }

    const std::string& Game::GetShortName() const
    {
        static std::string shortName = "T6";
        return shortName;
    }

    const std::vector<GameLanguagePrefix>& Game::GetLanguagePrefixes() const
    {
        static std::vector<GameLanguagePrefix> prefixes{
            {GameLanguage::LANGUAGE_ENGLISH,         "en_"},
            {GameLanguage::LANGUAGE_FRENCH,          "fr_"},
            {GameLanguage::LANGUAGE_FRENCH_CAN,      "fc_"},
            {GameLanguage::LANGUAGE_GERMAN,          "ge_"},
            {GameLanguage::LANGUAGE_AUSTRIAN,        "as_"},
            {GameLanguage::LANGUAGE_ITALIAN,         "it_"},
            {GameLanguage::LANGUAGE_SPANISH,         "sp_"},
            {GameLanguage::LANGUAGE_BRITISH,         "br_"},
            {GameLanguage::LANGUAGE_RUSSIAN,         "ru_"},
            {GameLanguage::LANGUAGE_POLISH,          "po_"},
            {GameLanguage::LANGUAGE_KOREAN,          "ko_"},
            {GameLanguage::LANGUAGE_JAPANESE,        "ja_"},
            {GameLanguage::LANGUAGE_CZECH,           "cz_"},
            {GameLanguage::LANGUAGE_FULL_JAPANESE,   "fj_"},
            {GameLanguage::LANGUAGE_PORTUGUESE,      "bp_"},
            {GameLanguage::LANGUAGE_MEXICAN_SPANISH, "ms_"},
        };

        return prefixes;
    }

    asset_type_t Game::GetAssetTypeCount() const
    {
        return ASSET_TYPE_COUNT;
    }

    std::optional<const char*> Game::GetAssetTypeName(const asset_type_t assetType) const
    {
        if (assetType < std::extent_v<decltype(ASSET_TYPE_NAMES)>)
            return ASSET_TYPE_NAMES[assetType];

        return std::nullopt;
    }

    asset_type_t Game::GetSubAssetTypeCount() const
    {
        return SUB_ASSET_TYPE_COUNT;
    }

    std::optional<const char*> Game::GetSubAssetTypeName(const asset_type_t subAssetType) const
    {
        if (subAssetType < std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)>)
            return SUB_ASSET_TYPE_NAMES[subAssetType];

        return std::nullopt;
    }
} // namespace T6
