#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace sound_curve
{
    std::unique_ptr<AssetCreator<IW3::AssetSoundCurve>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
} // namespace sound_curve
