#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

#include <memory>

namespace font_icon
{
    std::unique_ptr<IAssetDumper> CreateDumperT6(const AssetPool<T6::AssetFontIcon::Type>& pool);
} // namespace font_icon
