#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

#include <memory>

namespace font_icon
{
    std::unique_ptr<IAssetDumper<T6::FontIcon>> CreateDumperT6();
} // namespace font_icon
