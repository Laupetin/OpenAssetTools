#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/T6/T6.h"

#include <ostream>

namespace T6
{
    void DumpXModelAsJson(std::ostream& stream, const XModel* xmodel, AssetDumpingContext& context);
} // namespace T6
