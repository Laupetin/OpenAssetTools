#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace speaker_map
{
    std::unique_ptr<SubAssetCreator<IW3::SubAssetSpeakerMap>> CreateSpeakerMapLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
} // namespace speaker_map
