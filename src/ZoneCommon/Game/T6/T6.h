#pragma once

#include <d3d11.h>

#include "T6_Assets.h"

namespace T6
{
    struct ScriptStringList
    {
        int count;
        const char **strings;
    };

    struct XAsset
    {
        XAssetType type;
        XAssetHeader header;
    };


    struct XAssetList
    {
        ScriptStringList stringList;
        int dependCount;
        const char **depends;
        int assetCount;
        XAsset *assets;
    };
}