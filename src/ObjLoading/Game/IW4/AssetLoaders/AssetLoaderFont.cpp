#include "AssetLoaderFont.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderFont::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* font = memory->Create<Font_s>();
    memset(font, 0, sizeof(Font_s));
    font->fontName = memory->Dup(assetName.c_str());
    return font;
}
