#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/AssetRegistration.h"
#include "Game/T6/T6.h"
#include "Utils/MemoryManager.h"

#include <istream>

namespace T6
{
    bool LoadMaterialAsJson(
        std::istream& stream, Material& material, MemoryManager& memory, AssetCreationContext& context, AssetRegistration<AssetMaterial>& registration);
} // namespace T6
