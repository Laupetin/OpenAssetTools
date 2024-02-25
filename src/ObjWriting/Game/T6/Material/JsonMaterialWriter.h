#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/T6/T6.h"

#include <ostream>

namespace T6
{
    void DumpMaterialAsJson(std::ostream& stream, const Material* material, AssetDumpingContext& context);
} // namespace T6
