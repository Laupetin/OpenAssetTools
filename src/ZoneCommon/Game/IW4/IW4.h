#pragma once

#include <d3d11.h>
#include "Image/Texture.h"

#include "IW4_Assets.h"

namespace IW4
{
    struct DB_AuthHash
    {
        char bytes[32];
    };

    struct DB_AuthSignature
    {
        char bytes[256];
    };

    struct DB_AuthSubHeader
    {
        char fastfileName[32];
        unsigned int reserved;
        DB_AuthHash masterBlockHashes[244];
    };

    struct DB_AuthHeader
    {
        char magic[8];                                  // + 0
        unsigned int reserved;                          // + 8
        DB_AuthHash subheaderHash;                      // + 12
        DB_AuthSignature signedSubheaderHash;           // + 44
        DB_AuthSubHeader subheader;                     // + 300
    };

    struct ScriptStringList
    {
        int count;
        const char** strings;
    };

    struct XAsset
    {
        XAssetType type;
        XAssetHeader header;
    };

    struct XAssetList
    {
        ScriptStringList stringList;
        int assetCount;
        XAsset* assets;
    };
}