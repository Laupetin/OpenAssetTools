#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace vertex_decl
{
    std::unique_ptr<AssetCreator<IW4::AssetVertexDecl>> CreateLoaderIW4(MemoryManager& memory);
} // namespace vertex_decl
