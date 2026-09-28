#include "GameT4.h"

#include "T4.h"

#include <algorithm>

using namespace T4;

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
        "sound",
        "loadedsound",
        "clipmap_unused",
        "clipmap",
        "comworld",
        "gameworldsp",
        "gameworldmp",
        "mapents",
        "gfxworld",
        "lightdef",
        "uimap",
        "font",
        "menulist",
        "menu",
        "localize",
        "weapon",
        "snddriverglobals",
        "fx",
        "impactfx",
        "aitype",
        "mptype",
        "character",
        "xmodelalias",
        "rawfile",
        "stringtable",
        "packindex",
    };
    static_assert(std::extent_v<decltype(ASSET_TYPE_NAMES)> == ASSET_TYPE_COUNT);

    constexpr const char* SUB_ASSET_TYPE_NAMES[]{
        "technique",
        "vertexdecl",
        "vertexshader",
        "pixelshader",
        "accuracygraph",
        "flametable",
    };
    static_assert(std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)> == SUB_ASSET_TYPE_COUNT);
} // namespace

namespace T4
{
    Game::Game()
        : AbstractGame(ASSET_TYPE_NAMES, std::extent_v<decltype(ASSET_TYPE_NAMES)>, SUB_ASSET_TYPE_NAMES, std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)>)
    {
        AddAssetTypeNameAlias<AssetTechniqueSet>("techset");
        AddAssetTypeNameAlias<AssetLoadedSound>("loaded_sound");
        AddAssetTypeNameAlias<AssetClipMap>("col_map_sp");
        AddAssetTypeNameAlias<AssetClipMapPvs>("col_map_mp");
        AddAssetTypeNameAlias<AssetComWorld>("com_map");
        AddAssetTypeNameAlias<AssetGameWorldSp>("game_map_sp");
        AddAssetTypeNameAlias<AssetGameWorldMp>("game_map_mp");
        AddAssetTypeNameAlias<AssetMapEnts>("map_ents");
        AddAssetTypeNameAlias<AssetGfxWorld>("gfx_map");
        AddAssetTypeNameAlias<AssetLightDef>("gfxlightdef");
        AddAssetTypeNameAlias<AssetMenuList>("menufile");
    }

    GameId Game::GetId() const
    {
        return GameId::T4;
    }

    const std::string& Game::GetFullName() const
    {
        static std::string fullName = "Call Of Duty: World at War";
        return fullName;
    }

    const std::string& Game::GetShortName() const
    {
        static std::string shortName = "T4";
        return shortName;
    }
} // namespace T4
