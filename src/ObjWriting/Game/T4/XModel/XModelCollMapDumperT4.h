#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/T4/T4.h"

#include <string>

namespace xmodel
{
    void DumpXModelCollMapT4(const std::string& xmodelName, const AssetDumpingContext& context, const T4::PhysGeomList* collMap);
    void DumpXModelPhysCollMapT4(const std::string& xmodelName, const AssetDumpingContext& context, const T4::PhysGeomList* physGeoms);
} // namespace xmodel
