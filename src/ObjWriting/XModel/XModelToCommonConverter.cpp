#include "XModelToCommonConverter.h"

#include "Game/IW3/XModel/XModelToCommonConverterIW3.h"
#include "Game/IW4/XModel/XModelToCommonConverterIW4.h"
#include "Game/IW5/XModel/XModelToCommonConverterIW5.h"
#include "Game/QOS/XModel/XModelToCommonConverterQOS.h"
#include "Game/T4/XModel/XModelToCommonConverterT4.h"
#include "Game/T5/XModel/XModelToCommonConverterT5.h"
#include "Game/T6/XModel/XModelToCommonConverterT6.h"

#include <cassert>

namespace xmodel
{
    ToCommonConverter* ToCommonConverter::GetForGame(const GameId gameId)
    {
        static ToCommonConverter* toCommonConverters[]{
            new ToCommonConverterIW3(),
            new ToCommonConverterIW4(),
            new ToCommonConverterIW5(),
            new ToCommonConverterQOS(),
            new ToCommonConverterT4(),
            new ToCommonConverterT5(),
            new ToCommonConverterT6(),
        };
        static_assert(std::extent_v<decltype(toCommonConverters)> == static_cast<unsigned>(GameId::COUNT));
        assert(static_cast<unsigned>(gameId) < static_cast<unsigned>(GameId::COUNT));

        return toCommonConverters[std::to_underlying(gameId)];
    }
} // namespace xmodel
