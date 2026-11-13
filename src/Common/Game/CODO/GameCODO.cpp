#include "GameCODO.h"

#include "CODO.h"

#include <algorithm>

using namespace CODO;

namespace
{
    constexpr const char* ASSET_TYPE_NAMES[]{
        "physpreset", "physcollmap",  "xanim",        "xmodelsurfs",  "xmodel",          "material",         "pixelshader", "vertexshader",
        "vertexdecl", "techniqueset", "image",        "sound",        "soundcurve",      "loadedsound",      "aipaths",     "clipmap",
        "comworld",   "glassworld",   "vehicletrack", "mapents",      "fxworld",         "gfxworld",         "lightdef",    "uimap",
        "font",       "menulist",     "menu",         "localize",     "weapon",          "snddriverglobals", "fx",          "impactfx",
        "aitype",     "mptype",       "character",    "xmodelalias",  "rawfile",         "stringtable",      "leaderboard", "structureddatadef",
        "tracer",     "vehicle",      "addonmapents", "weaponattach", "netconststrings", "marketplacedata",
    };
    static_assert(std::extent_v<decltype(ASSET_TYPE_NAMES)> == ASSET_TYPE_COUNT);

    constexpr const char* SUB_ASSET_TYPE_NAMES[]{
        "technique",
        "accuracygraph",
    };
    static_assert(std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)> == SUB_ASSET_TYPE_COUNT);
} // namespace

namespace CODO
{
    Game::Game()
        : AbstractGame(ASSET_TYPE_NAMES, std::extent_v<decltype(ASSET_TYPE_NAMES)>, SUB_ASSET_TYPE_NAMES, std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)>)
    {
        AddAssetTypeNameAlias<AssetTechniqueSet>("techset");
        AddAssetTypeNameAlias<AssetLightDef>("gfxlightdef");
    }

    GameId Game::GetId() const
    {
        return GameId::CODO;
    }

    const std::string& Game::GetFullName() const
    {
        static std::string fullName = "Call Of Duty: Online";
        return fullName;
    }

    const std::string& Game::GetShortName() const
    {
        static std::string shortName = "CODO";
        return shortName;
    }
} // namespace CODO
