#include "IZoneWriterFactory.h"

#include "Game/IW3/ZoneWriterFactoryIW3.h"
#include "Game/IW4/ZoneWriterFactoryIW4.h"
#include "Game/IW5/ZoneWriterFactoryIW5.h"
#include "Game/T5/ZoneWriterFactoryT5.h"
#include "Game/T6/ZoneWriterFactoryT6.h"

#include <cassert>

const IZoneWriterFactory* IZoneWriterFactory::GetZoneWriterFactoryForGame(GameId game)
{
    static const IZoneWriterFactory* zoneCreators[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::ZoneWriterFactory(),
        new IW4::ZoneWriterFactory(),
        new IW5::ZoneWriterFactory(),
        new T5::ZoneWriterFactory(),
        new T6::ZoneWriterFactory(),
    };
    static_assert(std::extent_v<decltype(zoneCreators)> == static_cast<unsigned>(GameId::COUNT));

    assert(static_cast<unsigned>(game) < static_cast<unsigned>(GameId::COUNT));
    const auto* result = zoneCreators[static_cast<unsigned>(game)];
    assert(result);

    return result;
}
