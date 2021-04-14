#pragma once

#include "Loading/AssetLoadingActions.h"
#include "Game/IW3/IW3.h"

namespace IW3
{
    class Actions_LoadedSound final : public AssetLoadingActions
    {
    public:
        explicit Actions_LoadedSound(Zone* zone);

        void SetSoundData(MssSound* sound) const;
    };
}
