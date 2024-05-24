#include "AssetLoaderVertexDecl.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderVertexDecl::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* vertexDecl = memory->Create<MaterialVertexDeclaration>();
    memset(vertexDecl, 0, sizeof(MaterialVertexDeclaration));
    vertexDecl->name = memory->Dup(assetName.c_str());
    return vertexDecl;
}
