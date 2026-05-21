#include "GameIW6.h"

#include "IW6.h"

#include <algorithm>

using namespace IW6;

namespace
{
    constexpr const char* ASSET_TYPE_NAMES[]{
        "physpreset",
        "physcollmap",
        "xanim",
        "xmodelsurfs",
        "xmodel",
        "material",
        "computeshader",
        "vertexshader",
        "hullshader",
        "domainshader",
        "pixelshader",
        "vertexdecl",
        "techniqueset",
        "image",
        "sound",
        "soundcurve",
        "lpfcurve",
        "reverbcurve",
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
        "animationclass",
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
        "netconststrings",
        "reverbpreset",
        "luafile",
        "scriptable",
        "colorization",
        "colorizationset",
        "tonemapping",
        "equipmentsoundtable",
        "vectorfield",
        "dopplerpreset",
        "particlesimanimation",
    };
    static_assert(std::extent_v<decltype(ASSET_TYPE_NAMES)> == ASSET_TYPE_COUNT);

    constexpr const char* SUB_ASSET_TYPE_NAMES[]{
        "technique",
    };
    static_assert(std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)> == SUB_ASSET_TYPE_COUNT);
} // namespace

namespace IW6
{
    Game::Game()
        : AbstractGame(ASSET_TYPE_NAMES, std::extent_v<decltype(ASSET_TYPE_NAMES)>, SUB_ASSET_TYPE_NAMES, std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)>)
    {
        AddAssetTypeNameAlias<AssetTechniqueSet>("techset");
        AddAssetTypeNameAlias<AssetLightDef>("gfxlightdef");
    }

    GameId Game::GetId() const
    {
        return GameId::IW6;
    }

    const std::string& Game::GetFullName() const
    {
        static std::string fullName = "Call Of Duty: Ghosts";
        return fullName;
    }

    const std::string& Game::GetShortName() const
    {
        static std::string shortName = "IW6";
        return shortName;
    }
} // namespace IW6
