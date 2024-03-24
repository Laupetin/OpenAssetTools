#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/T6/T6.h"

#include <ostream>

namespace T6
{
    void DumpWeaponCamoAsJson(std::ostream& stream, const WeaponCamo* weaponCamo);
} // namespace T6
