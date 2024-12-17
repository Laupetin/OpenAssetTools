#include "ZoneCreatorIW5.h"

#include "Game/IW5/IW5.h"

using namespace IW5;

GameId ZoneCreator::GetGameId() const
{
    return GameId::IW5;
}

asset_type_t ZoneCreator::GetImageAssetType() const
{
    return ASSET_TYPE_IMAGE;
}
