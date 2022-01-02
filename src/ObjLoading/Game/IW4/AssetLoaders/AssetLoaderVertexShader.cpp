#include "AssetLoaderVertexShader.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderVertexShader::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* vertexShader = memory->Create<MaterialVertexShader>();
    memset(vertexShader, 0, sizeof(MaterialVertexShader));
    vertexShader->name = memory->Dup(assetName.c_str());
    return vertexShader;
}
