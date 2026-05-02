#include "GameIW5.h"

#include "IW5.h"

#include <algorithm>

using namespace IW5;

namespace
{
    constexpr const char* ASSET_TYPE_NAMES[]{
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
    static_assert(std::extent_v<decltype(ASSET_TYPE_NAMES)> == ASSET_TYPE_COUNT);

    constexpr const char* SUB_ASSET_TYPE_NAMES[]{
        "technique",
    };
    static_assert(std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)> == SUB_ASSET_TYPE_COUNT);
} // namespace

namespace IW5
{
    Game::Game()
        : AbstractGame(ASSET_TYPE_NAMES, std::extent_v<decltype(ASSET_TYPE_NAMES)>, SUB_ASSET_TYPE_NAMES, std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)>)
    {
        AddAssetTypeNameAlias<AssetTechniqueSet>("techset");
        AddAssetTypeNameAlias<AssetLightDef>("gfxlightdef");
    }

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
} // namespace IW5
