#include "ZoneCreatorIW3.h"

#include "Game/IW3/IW3.h"

using namespace IW3;

GameId ZoneCreator::GetGameId() const
{
    return GameId::IW3;
}

asset_type_t ZoneCreator::GetImageAssetType() const
{
    return ASSET_TYPE_IMAGE;
}
