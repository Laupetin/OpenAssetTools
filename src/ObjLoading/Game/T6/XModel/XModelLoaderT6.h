#pragma once

#include "AssetLoading/IAssetLoadingManager.h"
#include "Game/T6/T6.h"
#include "Utils/MemoryManager.h"

#include <istream>
#include <vector>

namespace T6
{
    bool LoadXModel(std::istream& stream, XModel& xmodel, MemoryManager* memory, IAssetLoadingManager* manager, std::vector<XAssetInfoGeneric*>& dependencies);
}
