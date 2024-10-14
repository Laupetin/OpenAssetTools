#include "ZoneCreatorT5.h"

#include "Game/T5/T5.h"

using namespace T5;

GameId ZoneCreator::GetGameId() const
{
    return GameId::T5;
}

asset_type_t ZoneCreator::GetImageAssetType() const
{
    return ASSET_TYPE_IMAGE;
}
