#include "ZoneCreatorIW4.h"

#include "Game/IW4/IW4.h"

using namespace IW4;

GameId ZoneCreator::GetGameId() const
{
    return GameId::IW4;
}

asset_type_t ZoneCreator::GetImageAssetType() const
{
    return ASSET_TYPE_IMAGE;
}
