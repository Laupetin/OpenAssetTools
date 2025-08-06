#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace font_icon
{
    std::unique_ptr<AssetCreator<T6::AssetFontIcon>> CreateCsvLoaderT6(MemoryManager& memory, ISearchPath& searchPath);
} // namespace font_icon
