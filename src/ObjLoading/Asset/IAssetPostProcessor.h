#pragma once

#include "AssetCreationContext.h"
#include "AssetCreationResult.h"
#include "Game/IAsset.h"
#include "Pool/XAssetInfo.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Zone/ZoneTypes.h"

#include <optional>
#include <string>

class AssetCreationContext;

class IAssetPostProcessor
{
public:
    IAssetPostProcessor() = default;
    virtual ~IAssetPostProcessor() = default;
    IAssetPostProcessor(const IAssetPostProcessor& other) = default;
    IAssetPostProcessor(IAssetPostProcessor&& other) noexcept = default;
    IAssetPostProcessor& operator=(const IAssetPostProcessor& other) = default;
    IAssetPostProcessor& operator=(IAssetPostProcessor&& other) noexcept = default;

    [[nodiscard]] virtual asset_type_t GetHandlingAssetType() const = 0;
    virtual void PostProcessAsset(XAssetInfoGeneric& assetInfo, AssetCreationContext& context) = 0;
    virtual void FinalizeZone(AssetCreationContext& context){};
};
