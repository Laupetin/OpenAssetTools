#include "ZoneContent.h"

ZoneContent::GameAsset::GameAsset(std::string assetTypeName, std::string assetName)
{
    m_asset_type_name = std::move(assetTypeName);
    m_asset_name = std::move(assetName);
}

ZoneContent::ZoneContent()
{
    m_game_name = "";
    m_assets = std::vector<GameAsset>();
}