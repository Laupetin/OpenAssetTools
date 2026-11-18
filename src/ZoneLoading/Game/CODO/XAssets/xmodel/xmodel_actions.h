#pragma once

#include "Game/CODO/CODO.h"
#include "Loading/AssetLoadingActions.h"

namespace CODO
{
    class Actions_XModel final : public AssetLoadingActions
    {
    public:
        explicit Actions_XModel(Zone& zone);

        void SetModelSurfs(XModelLodInfo* lodInfo, XModelSurfs* modelSurfs) const;
    };
} // namespace CODO
