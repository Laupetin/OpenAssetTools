#pragma once

#include "AssetLoading/IAssetLoadingManager.h"
#include "Game/IW5/IW5.h"
#include "Utils/MemoryManager.h"

#include <istream>

namespace IW5
{
    bool LoadMaterialAsJson(
        std::istream& stream, Material& material, MemoryManager* memory, IAssetLoadingManager* manager, std::vector<XAssetInfoGeneric*>& dependencies);
} // namespace IW5
