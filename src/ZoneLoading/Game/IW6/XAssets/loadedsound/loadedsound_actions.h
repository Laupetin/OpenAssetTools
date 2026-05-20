#pragma once

#include "Game/IW6/IW6.h"
#include "Loading/AssetLoadingActions.h"

namespace IW6
{
    class Actions_LoadedSound final : public AssetLoadingActions
    {
    public:
        explicit Actions_LoadedSound(Zone& zone);

        void SetSoundData(XA2Sound* sound) const;
    };
} // namespace IW6
