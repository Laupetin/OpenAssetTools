#include "AssetLoaderVertexShader.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderVertexShader::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* vertexShader = memory->Create<MaterialVertexShader>();
    memset(vertexShader, 0, sizeof(MaterialVertexShader));
    vertexShader->name = memory->Dup(assetName.c_str());
    return vertexShader;
}
