#include "IGame.h"

#include "IW3/GameIW3.h"
#include "IW4/GameIW4.h"
#include "IW5/GameIW5.h"
#include "T5/GameT5.h"
#include "T6/GameT6.h"

#include <cassert>

IGame* IGame::GetGameById(GameId gameId)
{
    static IGame* games[static_cast<unsigned>(GameId::COUNT)]{
        &g_GameIW3,
        &g_GameIW4,
        &g_GameIW5,
        &g_GameT5,
        &g_GameT6,
    };

    assert(static_cast<unsigned>(gameId) < static_cast<unsigned>(GameId::COUNT));
    auto* result = games[static_cast<unsigned>(gameId)];
    assert(result);

    return result;
}
