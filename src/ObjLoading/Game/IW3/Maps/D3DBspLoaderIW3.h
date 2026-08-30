#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"

#include <memory>

namespace map_d3dbsp
{
    std::unique_ptr<AssetCreator<IW3::AssetClipMapPvs>> CreateClipMapPvsLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
    std::unique_ptr<AssetCreator<IW3::AssetMapEnts>> CreateMapEntsLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
    std::unique_ptr<AssetCreator<IW3::AssetComWorld>> CreateComWorldLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
    std::unique_ptr<AssetCreator<IW3::AssetGameWorldSp>> CreateGameWorldSpLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
    std::unique_ptr<AssetCreator<IW3::AssetGameWorldMp>> CreateGameWorldMpLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
    std::unique_ptr<AssetCreator<IW3::AssetGfxWorld>> CreateGfxWorldLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
} // namespace map_d3dbsp
