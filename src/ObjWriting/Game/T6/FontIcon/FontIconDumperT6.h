#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6::font_icon
{
    std::unique_ptr<IAssetDumper<FontIcon>> CreateDumper();
} // namespace T6::font_icon
