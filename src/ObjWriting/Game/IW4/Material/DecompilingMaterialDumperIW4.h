#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/IW4/IW4.h"
#include "Obj/Gdt/GdtStream.h"

namespace IW4
{
    void DecompileMaterialToGdt(GdtOutputStream& out, const Material& material, AssetDumpingContext& context);
} // namespace IW4
