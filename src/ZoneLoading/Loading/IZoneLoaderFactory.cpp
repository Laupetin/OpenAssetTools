#include "IZoneLoaderFactory.h"

#include "Game/IW3/ZoneLoaderFactoryIW3.h"
#include "Game/IW4/ZoneLoaderFactoryIW4.h"
#include "Game/IW5/ZoneLoaderFactoryIW5.h"
#include "Game/T5/ZoneLoaderFactoryT5.h"
#include "Game/T6/ZoneLoaderFactoryT6.h"

#include <cassert>

const IZoneLoaderFactory* IZoneLoaderFactory::GetZoneLoaderFactoryForGame(GameId game)
{
    static const IZoneLoaderFactory* zoneCreators[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::ZoneLoaderFactory(),
        new IW4::ZoneLoaderFactory(),
        new IW5::ZoneLoaderFactory(),
        new T5::ZoneLoaderFactory(),
        new T6::ZoneLoaderFactory(),
    };
    static_assert(std::extent_v<decltype(zoneCreators)> == static_cast<unsigned>(GameId::COUNT));

    assert(static_cast<unsigned>(game) < static_cast<unsigned>(GameId::COUNT));
    const auto* result = zoneCreators[static_cast<unsigned>(game)];
    assert(result);

    return result;
}
