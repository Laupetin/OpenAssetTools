#include "GameIW3.h"

#include "IW3.h"

#include <algorithm>

using namespace IW3;

namespace
{
    constexpr const char* ASSET_TYPE_NAMES[]{
        "xmodelpieces",   "physpreset", "xanim",       "xmodel",      "material",         "techniqueset", "image",    "sound",    "soundcurve", "loadedsound",
        "clipmap_unused", "clipmap",    "comworld",    "gameworldsp", "gameworldmp",      "mapents",      "gfxworld", "lightdef", "uimap",      "font",
        "menulist",       "menu",       "localize",    "weapon",      "snddriverglobals", "fx",           "impactfx", "aitype",   "mptype",     "character",
        "xmodelalias",    "rawfile",    "stringtable",
    };
    static_assert(std::extent_v<decltype(ASSET_TYPE_NAMES)> == ASSET_TYPE_COUNT);

    constexpr const char* SUB_ASSET_TYPE_NAMES[]{
        "technique",
        "vertexdecl",
        "vertexshader",
        "pixelshader",
    };
    static_assert(std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)> == SUB_ASSET_TYPE_COUNT);
} // namespace

namespace IW3
{
    Game::Game()
        : AbstractGame(ASSET_TYPE_NAMES, std::extent_v<decltype(ASSET_TYPE_NAMES)>, SUB_ASSET_TYPE_NAMES, std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)>)
    {
        AddAssetTypeNameAlias<AssetTechniqueSet>("techset");
        AddAssetTypeNameAlias<AssetLightDef>("gfxlightdef");
    }

    GameId Game::GetId() const
    {
        return GameId::IW3;
    }

    const std::string& Game::GetFullName() const
    {
        static std::string fullName = "Call Of Duty 4: Modern Warfare";
        return fullName;
    }

    const std::string& Game::GetShortName() const
    {
        static std::string shortName = "IW3";
        return shortName;
    }
} // namespace IW3
