#pragma once

#include "Loading/AssetLoadingActions.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class Actions_XModel final : public AssetLoadingActions
    {
    public:
        explicit Actions_XModel(Zone* zone);

        void SetModelSurfs(XModelLodInfo* lodInfo, XModelSurfs* modelSurfs) const;
    };
}
