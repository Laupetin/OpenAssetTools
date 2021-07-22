#pragma once

#include "Loading/AssetLoadingActions.h"
#include "Game/IW5/IW5.h"

namespace IW5
{
    class Actions_LoadedSound final : public AssetLoadingActions
    {
    public:
        explicit Actions_LoadedSound(Zone* zone);

        void SetSoundData(MssSound* sound) const;
    };
}
