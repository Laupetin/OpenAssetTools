#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace sound_alias
{
    std::unique_ptr<SubAssetCreator<IW3::SubAssetSpeakerMap>> CreateSpeakerMapLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
} // namespace sound_alias
