#include "CommonAsset.h"

#include "IW3/CommonAssetIW3.h"
#include "IW4/CommonAssetIW4.h"
#include "IW5/CommonAssetIW5.h"
#include "T5/CommonAssetT5.h"
#include "T6/CommonAssetT6.h"

#include <cassert>

ICommonAssetTypeMapper* ICommonAssetTypeMapper::GetCommonAssetMapperByGame(GameId gameId)
{
    static ICommonAssetTypeMapper* assetTypeMappers[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::CommonAssetTypeMapper(),
        new IW4::CommonAssetTypeMapper(),
        new IW5::CommonAssetTypeMapper(),
        new T5::CommonAssetTypeMapper(),
        new T6::CommonAssetTypeMapper(),
    };

    assert(static_cast<unsigned>(gameId) < static_cast<unsigned>(GameId::COUNT));
    auto* result = assetTypeMappers[static_cast<unsigned>(gameId)];
    assert(result);

    return result;
}
