#pragma once
#include "AssetLoading/IAssetLoadingManager.h"
#include "IAssetLoadingManager.h"
#include "IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/ClassUtils.h"
#include "Utils/MemoryManager.h"
#include "Zone/ZoneTypes.h"

#include <string>

class IAssetCompiler
{
public:
    IAssetCompiler() = default;
    virtual ~IAssetCompiler() = default;
    IAssetCompiler(const IAssetCompiler& other) = default;
    IAssetCompiler(IAssetCompiler&& other) noexcept = default;
    IAssetCompiler& operator=(const IAssetCompiler& other) = default;
    IAssetCompiler& operator=(IAssetCompiler&& other) noexcept = default;

    _NODISCARD virtual asset_type_t GetHandlingAssetType() const = 0;

    virtual bool LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) = 0;

    virtual void FinalizeAssetsForZone(AssetLoadingContext& context) const
    {
        // Do nothing by default
    }
};
