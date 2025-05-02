#pragma once

#include "Game/IW4/IW4.h"
#include "Loading/AssetLoadingActions.h"

namespace IW4
{
    class Actions_XModel final : public AssetLoadingActions
    {
    public:
        explicit Actions_XModel(Zone& zone);

        void SetModelSurfs(XModelLodInfo* lodInfo, XModelSurfs* modelSurfs) const;
    };
} // namespace IW4
