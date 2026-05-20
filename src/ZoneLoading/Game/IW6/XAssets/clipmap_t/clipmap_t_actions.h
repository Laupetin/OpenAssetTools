#pragma once

#include "Game/IW6/IW6.h"
#include "Loading/AssetLoadingActions.h"

namespace IW6
{
    class Actions_clipMap_t final : public AssetLoadingActions
    {
    public:
        explicit Actions_clipMap_t(Zone& zone);

        void ReallocClipInfo(const ClipInfo* clipInfo, clipMap_t* clipMap) const;
    };
} // namespace IW6
