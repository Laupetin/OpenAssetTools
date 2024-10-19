#include "IObjLoader.h"

#include "Game/IW3/ObjLoaderIW3.h"
#include "Game/IW4/ObjLoaderIW4.h"
#include "Game/IW5/ObjLoaderIW5.h"
#include "Game/T5/ObjLoaderT5.h"
#include "Game/T6/ObjLoaderT6.h"

#include <cassert>

const IObjLoader* IObjLoader::GetObjLoaderForGame(GameId game)
{
    static const IObjLoader* zoneCreators[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::ObjLoader(),
        new IW4::ObjLoader(),
        new IW5::ObjLoader(),
        new T5::ObjLoader(),
        new T6::ObjLoader(),
    };
    static_assert(std::extent_v<decltype(zoneCreators)> == static_cast<unsigned>(GameId::COUNT));

    assert(static_cast<unsigned>(game) < static_cast<unsigned>(GameId::COUNT));
    const auto* result = zoneCreators[static_cast<unsigned>(game)];
    assert(result);

    return result;
}
