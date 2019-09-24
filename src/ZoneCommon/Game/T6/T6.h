#pragma once

#include <d3d11.h>

namespace T6
{
#include "T6_Assets.h"

    enum XFileBlock
    {
	    XFILE_BLOCK_TEMP,
	    XFILE_BLOCK_RUNTIME_VIRTUAL,
	    XFILE_BLOCK_RUNTIME_PHYSICAL,
	    XFILE_BLOCK_DELAY_VIRTUAL,
	    XFILE_BLOCK_DELAY_PHYSICAL,
	    XFILE_BLOCK_VIRTUAL,
	    XFILE_BLOCK_PHYSICAL,
	    XFILE_BLOCK_STREAMER_RESERVE,
	    MAX_XFILE_COUNT,
    };

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