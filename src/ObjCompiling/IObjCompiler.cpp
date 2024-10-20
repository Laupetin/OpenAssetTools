#include "IObjCompiler.h"

#include "Game/IW3/ObjCompilerIW3.h"
#include "Game/IW4/ObjCompilerIW4.h"
#include "Game/IW5/ObjCompilerIW5.h"
#include "Game/T5/ObjCompilerT5.h"
#include "Game/T6/ObjCompilerT6.h"

#include <cassert>

const IObjCompiler* IObjCompiler::GetObjCompilerForGame(GameId game)
{
    static const IObjCompiler* objCompilers[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::ObjCompiler(),
        new IW4::ObjCompiler(),
        new IW5::ObjCompiler(),
        new T5::ObjCompiler(),
        new T6::ObjCompiler(),
    };
    static_assert(std::extent_v<decltype(objCompilers)> == static_cast<unsigned>(GameId::COUNT));

    assert(static_cast<unsigned>(game) < static_cast<unsigned>(GameId::COUNT));
    const auto* result = objCompilers[static_cast<unsigned>(game)];
    assert(result);

    return result;
}
