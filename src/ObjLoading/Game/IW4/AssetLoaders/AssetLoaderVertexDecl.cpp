#include "AssetLoaderVertexDecl.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderVertexDecl::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* vertexDecl = memory->Create<MaterialVertexDeclaration>();
    memset(vertexDecl, 0, sizeof(MaterialVertexDeclaration));
    vertexDecl->name = memory->Dup(assetName.c_str());
    return vertexDecl;
}
