#pragma once

#include "Game/IW5/IW5.h"
#include "Loading/AssetLoadingActions.h"

namespace IW5
{
    class Actions_XModel final : public AssetLoadingActions
    {
    public:
        explicit Actions_XModel(Zone* zone);

        void SetModelSurfs(XModelLodInfo* lodInfo, XModelSurfs* modelSurfs) const;
    };
} // namespace IW5
