#pragma once

#include "Game/IW4/IW4.h"
#include "Loading/AssetLoadingActions.h"

namespace IW4
{
    class Actions_LoadedSound final : public AssetLoadingActions
    {
    public:
        explicit Actions_LoadedSound(Zone& zone);

        void SetSoundData(MssSound* sound) const;
    };
} // namespace IW4
