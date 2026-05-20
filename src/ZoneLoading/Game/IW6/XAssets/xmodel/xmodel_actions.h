#pragma once

#include "Game/IW6/IW6.h"
#include "Loading/AssetLoadingActions.h"

namespace IW6
{
    class Actions_XModel final : public AssetLoadingActions
    {
    public:
        explicit Actions_XModel(Zone& zone);

        void SetModelSurfs(XModelLodInfo* lodInfo, XModelSurfs* modelSurfs) const;
    };
} // namespace IW6
