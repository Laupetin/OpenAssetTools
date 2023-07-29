#pragma once

#include "Decompiling/DecompilingContext.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class DecompilerMaterial
    {
    protected:
        void DumpAsset(DecompilingContext& context, XAssetInfo<Material>* asset);
    };
}
