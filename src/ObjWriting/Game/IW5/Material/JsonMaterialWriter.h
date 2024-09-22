#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/IW5/IW5.h"

#include <ostream>

namespace IW5
{
    void DumpMaterialAsJson(std::ostream& stream, const Material* material, AssetDumpingContext& context);
} // namespace IW5
