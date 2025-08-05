#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace sound_curve
{
    std::unique_ptr<AssetCreator<IW4::AssetSoundCurve>> CreateLoaderIW4(MemoryManager& memory, ISearchPath& searchPath);
} // namespace sound_curve
