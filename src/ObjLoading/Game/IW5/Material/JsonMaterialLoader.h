#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/AssetRegistration.h"
#include "Game/IW5/IW5.h"
#include "Utils/MemoryManager.h"

#include <istream>

namespace IW5
{
    bool LoadMaterialAsJson(
        std::istream& stream, Material& material, MemoryManager& memory, AssetCreationContext& context, AssetRegistration<AssetMaterial>& registration);
} // namespace IW5
