#pragma once

#include "Game/T4/T4.h"
#include "Loading/AssetLoadingActions.h"

namespace T4
{
    class Actions_LoadedSound final : public AssetLoadingActions
    {
    public:
        explicit Actions_LoadedSound(Zone& zone);

        void SetSoundData(snd_asset* sound) const;
    };
} // namespace T4
