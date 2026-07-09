#include "GameQOS.h"

#include "QOS.h"

#include <type_traits>

using namespace QOS;

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
        "sndcurve",
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
        "xmltree",
        "scene_animation",
        "cutscene",
        "custom_camera",
    };
    static_assert(std::extent_v<decltype(ASSET_TYPE_NAMES)> == ASSET_TYPE_COUNT);

    constexpr const char* SUB_ASSET_TYPE_NAMES[]{
        "technique",
        "vertexdecl",
        "vertexshader",
        "pixelshader",
        "accuracygraph",
    };
    static_assert(std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)> == SUB_ASSET_TYPE_COUNT);
} // namespace

namespace QOS
{
    Game::Game()
        : AbstractGame(ASSET_TYPE_NAMES, std::extent_v<decltype(ASSET_TYPE_NAMES)>, SUB_ASSET_TYPE_NAMES, std::extent_v<decltype(SUB_ASSET_TYPE_NAMES)>)
    {
        AddAssetTypeNameAlias<AssetTechniqueSet>("techniqueset");
        AddAssetTypeNameAlias<AssetClipMapSp>("clipmap_sp");
        AddAssetTypeNameAlias<AssetClipMapMp>("clipmap_mp");
        AddAssetTypeNameAlias<AssetClipMapSp>("clipmap");
        AddAssetTypeNameAlias<AssetComWorld>("comworld");
        AddAssetTypeNameAlias<AssetGameWorldSp>("gameworldsp");
        AddAssetTypeNameAlias<AssetGameWorldMp>("gameworldmp");
        AddAssetTypeNameAlias<AssetMapEnts>("mapents");
        AddAssetTypeNameAlias<AssetGfxWorld>("gfxworld");
        AddAssetTypeNameAlias<AssetLightDef>("gfxlightdef");
        AddAssetTypeNameAlias<AssetMenuFile>("menulist");
        AddAssetTypeNameAlias<AssetLocalize>("localizeentry");
    }

    GameId Game::GetId() const
    {
        return GameId::QOS;
    }

    const std::string& Game::GetFullName() const
    {
        static std::string fullName = "007: Quantum of Solace";
        return fullName;
    }

    const std::string& Game::GetShortName() const
    {
        static std::string shortName = "QOS";
        return shortName;
    }
} // namespace QOS
