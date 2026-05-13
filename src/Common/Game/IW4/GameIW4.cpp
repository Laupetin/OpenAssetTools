#include "GameIW4.h"

#include "IW4.h"

#include <algorithm>

using namespace IW4;

namespace
{
    constexpr const char* ASSET_TYPE_NAMES[]{
        "physpreset",  "physcollmap", "xanim",        "xmodelsurfs", "xmodel",
        "material",    "pixelshader", "vertexshader", "vertexdecl",  "techniqueset",
        "image",       "sound",       "soundcurve",   "loaded_sound", "clipmap_unused",
        "clipmap",     "comworld",    "gameworldsp",  "gameworldmp", "mapents",
        "fxworld",     "gfxworld",    "lightdef",     "uimap",       "font",
        "menulist",    "menu",        "localize",     "weapon",      "snddriverglobals",
        "fx",          "impactfx",    "aitype",       "mptype",      "character",
        "xmodelalias", "rawfile",     "stringtable",  "leaderboard", "structureddatadef",
        "tracer",      "vehicle",     "addonmapents",
    };
    static_assert(std::extent_v<decltype(ASSET_TYPE_NAMES)> == ASSET_TYPE_COUNT);

    constexpr const char* SUB_ASSET_TYPE_NAMES[]{
        "technique",
    };
    static_assert(std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)> == SUB_ASSET_TYPE_COUNT);
} // namespace

namespace IW4
{
    Game::Game()
        : AbstractGame(ASSET_TYPE_NAMES, std::extent_v<decltype(ASSET_TYPE_NAMES)>, SUB_ASSET_TYPE_NAMES, std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)>)
    {
        AddAssetTypeNameAlias<AssetTechniqueSet>("techset");
        AddAssetTypeNameAlias<AssetLightDef>("gfxlightdef");
    }

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
} // namespace IW4
