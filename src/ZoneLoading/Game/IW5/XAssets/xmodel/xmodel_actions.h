#pragma once

#include "Loading/AssetLoadingActions.h"
#include "Game/IW5/IW5.h"

namespace IW5
{
    class Actions_XModel final : public AssetLoadingActions
    {
    public:
        explicit Actions_XModel(Zone* zone);

        void SetModelSurfs(XModelLodInfo* lodInfo, XModelSurfs* modelSurfs) const;
    };
}
