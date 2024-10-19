#include "IObjWriter.h"

#include "Game/IW3/ObjWriterIW3.h"
#include "Game/IW4/ObjWriterIW4.h"
#include "Game/IW5/ObjWriterIW5.h"
#include "Game/T5/ObjWriterT5.h"
#include "Game/T6/ObjWriterT6.h"

#include <cassert>

const IObjWriter* IObjWriter::GetObjWriterForGame(GameId game)
{
    static const IObjWriter* zoneCreators[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::ObjWriter(),
        new IW4::ObjWriter(),
        new IW5::ObjWriter(),
        new T5::ObjWriter(),
        new T6::ObjWriter(),
    };
    static_assert(std::extent_v<decltype(zoneCreators)> == static_cast<unsigned>(GameId::COUNT));

    assert(static_cast<unsigned>(game) < static_cast<unsigned>(GameId::COUNT));
    const auto* result = zoneCreators[static_cast<unsigned>(game)];
    assert(result);

    return result;
}
