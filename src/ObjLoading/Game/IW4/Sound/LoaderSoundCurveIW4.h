#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW4::sound_curve
{
    std::unique_ptr<AssetCreator<AssetSoundCurve>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace IW4::sound_curve
