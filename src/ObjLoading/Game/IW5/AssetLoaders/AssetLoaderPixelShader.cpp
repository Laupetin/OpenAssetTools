#include "AssetLoaderPixelShader.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderPixelShader::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* pixelShader = memory->Create<MaterialPixelShader>();
    memset(pixelShader, 0, sizeof(MaterialPixelShader));
    pixelShader->name = memory->Dup(assetName.c_str());
    return pixelShader;
}
