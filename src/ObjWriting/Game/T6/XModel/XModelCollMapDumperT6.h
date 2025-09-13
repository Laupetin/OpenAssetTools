#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace xmodel
{
    void DumpCollMap(const std::string& collMapName, const AssetDumpingContext& context, const T6::Collmap& collMap);
} // namespace xmodel
