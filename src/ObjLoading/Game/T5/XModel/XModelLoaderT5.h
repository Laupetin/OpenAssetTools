#pragma once

#include "AssetLoading/IAssetLoadingManager.h"
#include "Game/T5/T5.h"
#include "Utils/MemoryManager.h"

#include <istream>
#include <vector>

namespace T5
{
    bool LoadXModel(std::istream& stream, XModel& xmodel, MemoryManager* memory, IAssetLoadingManager* manager, std::vector<XAssetInfoGeneric*>& dependencies);
}
