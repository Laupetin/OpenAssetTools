#include "GameIW5.h"

#include "IW5.h"

#include <algorithm>

using namespace IW5;

namespace
{
    constexpr const char* ASSET_TYPE_NAMES[ASSET_TYPE_COUNT]{
        "physpreset",
        "physcollmap",
        "xanim",
        "xmodelsurfs",
        "xmodel",
        "material",
        "pixelshader",
        "vertexshader",
        "vertexdecl",
        "techniqueset",
        "image",
        "sound",
        "soundcurve",
        "loadedsound",
        "clipmap",
        "comworld",
        "glassworld",
        "pathdata",
        "vehicletrack",
        "mapents",
        "fxworld",
        "gfxworld",
        "lightdef",
        "uimap",
        "font",
        "menulist",
        "menu",
        "localize",
        "attachment",
        "weapon",
        "snddriverglobals",
        "fx",
        "impactfx",
        "surfacefx",
        "aitype",
        "mptype",
        "character",
        "xmodelalias",
        "rawfile",
        "scriptfile",
        "stringtable",
        "leaderboard",
        "structureddatadef",
        "tracer",
        "vehicle",
        "addonmapents",
    };
} // namespace

namespace IW5
{
    GameId Game::GetId() const
    {
        return GameId::IW5;
    }

    const std::string& Game::GetFullName() const
    {
        static std::string fullName = "Call Of Duty: Modern Warfare 3";
        return fullName;
    }

    const std::string& Game::GetShortName() const
    {
        static std::string shortName = "IW5";
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
} // namespace IW5
