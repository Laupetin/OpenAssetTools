#pragma once

#include "Loading/AssetLoadingActions.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class Actions_LoadedSound final : public AssetLoadingActions
    {
    public:
        explicit Actions_LoadedSound(Zone* zone);

        void SetSoundData(MssSound* sound) const;
    };
}
