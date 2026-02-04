#include "GameIW4.h"

#include "IW4.h"

#include <algorithm>

using namespace IW4;

namespace
{
    constexpr const char* ASSET_TYPE_NAMES[ASSET_TYPE_COUNT]{
        "physpreset",  "physcollmap", "xanim",        "xmodelsurfs", "xmodel",
        "material",    "pixelshader", "vertexshader", "vertexdecl",  "techniqueset",
        "image",       "sound",       "soundcurve",   "loadedsound", "clipmap_unused",
        "clipmap",     "comworld",    "gameworldsp",  "gameworldmp", "mapents",
        "fxworld",     "gfxworld",    "lightdef",     "uimap",       "font",
        "menulist",    "menu",        "localize",     "weapon",      "snddriverglobals",
        "fx",          "impactfx",    "aitype",       "mptype",      "character",
        "xmodelalias", "rawfile",     "stringtable",  "leaderboard", "structureddatadef",
        "tracer",      "vehicle",     "addonmapents",
    };
} // namespace

namespace IW4
{
    GameId Game::GetId() const
    {
        return GameId::IW4;
    }

    const std::string& Game::GetFullName() const
    {
        static std::string fullName = "Call Of Duty: Modern Warfare 2";
        return fullName;
    }

    const std::string& Game::GetShortName() const
    {
        static std::string shortName = "IW4";
        return shortName;
    }

    const std::vector<GameLanguagePrefix>& Game::GetLanguagePrefixes() const
    {
        static std::vector<GameLanguagePrefix> prefixes;
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
} // namespace IW4
