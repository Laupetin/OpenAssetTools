#include "ImageToCommonConverter.h"

#include "Game/IW3/Image/ImageToCommonConverterIW3.h"
#include "Game/IW4/Image/ImageToCommonConverterIW4.h"
#include "Game/IW5/Image/ImageToCommonConverterIW5.h"
#include "Game/T4/Image/ImageToCommonConverterT4.h"
#include "Game/T5/Image/ImageToCommonConverterT5.h"
#include "Game/T6/Image/ImageToCommonConverterT6.h"

#include <cassert>

namespace image
{
    ToCommonConverter* ToCommonConverter::GetForGame(const GameId gameId)
    {
        static ToCommonConverter* toCommonConverters[]{
            new ToCommonConverterIW3(),
            new ToCommonConverterIW4(),
            new ToCommonConverterIW5(),
            new ToCommonConverterT4(),
            new ToCommonConverterT5(),
            new ToCommonConverterT6(),
        };
        static_assert(std::extent_v<decltype(toCommonConverters)> == static_cast<unsigned>(GameId::COUNT));
        assert(static_cast<unsigned>(gameId) < static_cast<unsigned>(GameId::COUNT));

        return toCommonConverters[std::to_underlying(gameId)];
    }
} // namespace image
