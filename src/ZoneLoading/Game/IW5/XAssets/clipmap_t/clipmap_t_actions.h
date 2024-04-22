#pragma once

#include "Game/IW5/IW5.h"
#include "Loading/AssetLoadingActions.h"

namespace IW5
{
    class Actions_clipMap_t final : public AssetLoadingActions
    {
    public:
        explicit Actions_clipMap_t(Zone* zone);

        void ReallocClipInfo(const ClipInfo* clipInfo, clipMap_t* clipMap) const;
    };
} // namespace IW5
