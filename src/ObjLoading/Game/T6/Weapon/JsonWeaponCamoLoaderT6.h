#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/AssetRegistration.h"
#include "Game/T6/T6.h"
#include "Utils/MemoryManager.h"

#include <istream>

namespace T6
{
    bool LoadWeaponCamoAsJson(
        std::istream& stream, WeaponCamo& weaponCamo, MemoryManager& memory, AssetCreationContext& context, AssetRegistration<AssetWeaponCamo>& registration);
} // namespace T6
