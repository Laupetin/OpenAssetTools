#pragma once

#include "Asset/IAssetCreator.h"
#include "Asset/IZoneAssetCreationState.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Zone/Definition/ZoneDefinition.h"
#include "Zone/Zone.h"

#include <memory>

namespace T6::key_value_pairs
{
    std::unique_ptr<IAssetCreator>
        CreateCompiler(MemoryManager& memory, const Zone& zone, const ZoneDefinition& zoneDefinition, ZoneAssetCreationStateContainer& zoneStates);
} // namespace T6::key_value_pairs
