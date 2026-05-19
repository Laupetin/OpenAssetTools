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
        "techset",
        "image",
        "sound",
        "loaded_sound",
        "col_map_sp",
        "col_map_mp",
        "com_map",
        "game_map_sp",
        "game_map_mp",
        "map_ents",
        "gfx_map",
        "lightdef",
        "ui_map",
        "font",
        "menufile",
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
    };
    static_assert(std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)> == SUB_ASSET_TYPE_COUNT);
} // namespace

namespace T4
{
    Game::Game()
        : AbstractGame(ASSET_TYPE_NAMES, std::extent_v<decltype(ASSET_TYPE_NAMES)>, SUB_ASSET_TYPE_NAMES, std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)>)
    {
        AddAssetTypeNameAlias<AssetTechniqueSet>("techset");
        AddAssetTypeNameAlias<AssetTechniqueSet>("techniqueset");
        AddAssetTypeNameAlias<AssetLoadedSound>("loadedsound");
        AddAssetTypeNameAlias<AssetClipMap>("clipmap_unused");
        AddAssetTypeNameAlias<AssetClipMap>("clipmap_sp");
        AddAssetTypeNameAlias<AssetClipMapPvs>("clipmap");
        AddAssetTypeNameAlias<AssetClipMapPvs>("clipmap_mp");
        AddAssetTypeNameAlias<AssetComWorld>("comworld");
        AddAssetTypeNameAlias<AssetGameWorldSp>("gameworldsp");
        AddAssetTypeNameAlias<AssetGameWorldMp>("gameworldmp");
        AddAssetTypeNameAlias<AssetMapEnts>("mapents");
        AddAssetTypeNameAlias<AssetGfxWorld>("gfxworld");
        AddAssetTypeNameAlias<AssetLightDef>("gfxlightdef");
        AddAssetTypeNameAlias<AssetMenuList>("menulist");
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
