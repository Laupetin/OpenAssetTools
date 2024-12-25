#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW4
{
    std::unique_ptr<AssetCreator<AssetVertexDecl>> CreateVertexDeclLoader(MemoryManager& memory);
} // namespace IW4
