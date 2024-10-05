#include "ZoneCreator.h"

#include "Game/IW3/ZoneCreatorIW3.h"
#include "Game/IW4/ZoneCreatorIW4.h"
#include "Game/IW5/ZoneCreatorIW5.h"
#include "Game/T5/ZoneCreatorT5.h"
#include "Game/T6/ZoneCreatorT6.h"

#include <cassert>

const IZoneCreator* IZoneCreator::GetCreatorForGame(GameId game)
{
    static const IZoneCreator* zoneCreators[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::ZoneCreator(),
        new IW4::ZoneCreator(),
        new IW5::ZoneCreator(),
        new T5::ZoneCreator(),
        new T6::ZoneCreator(),
    };

    assert(static_cast<unsigned>(game) < static_cast<unsigned>(GameId::COUNT));
    const auto* result = zoneCreators[static_cast<unsigned>(game)];
    assert(result);

    return result;
}
