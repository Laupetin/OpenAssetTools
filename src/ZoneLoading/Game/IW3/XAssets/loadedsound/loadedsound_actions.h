#pragma once

#include "Game/IW3/IW3.h"
#include "Loading/AssetLoadingActions.h"

namespace IW3
{
    class Actions_LoadedSound final : public AssetLoadingActions
    {
    public:
        explicit Actions_LoadedSound(Zone& zone);

        void SetSoundData(MssSound* sound) const;
    };
} // namespace IW3
