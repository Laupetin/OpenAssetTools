#pragma once
#include <string>
#include <vector>

class ZoneContent
{
public:
    class GameAsset
    {
    public:
        GameAsset(std::string assetTypeName, std::string assetName);

        std::string m_asset_type_name;
        std::string m_asset_name;
    };

    ZoneContent();

    std::string m_game_name;
    std::vector<GameAsset> m_assets;
};