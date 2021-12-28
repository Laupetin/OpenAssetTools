#pragma once
#include <string>

#include "Utils/ClassUtils.h"
#include "SearchPath/ISearchPath.h"
#include "IAssetLoadingManager.h"
#include "IGdtQueryable.h"
#include "Zone/ZoneTypes.h"

class IAssetLoader
{
public:
    IAssetLoader() = default;
    virtual ~IAssetLoader() = default;
    IAssetLoader(const IAssetLoader& other) = default;
    IAssetLoader(IAssetLoader&& other) noexcept = default;
    IAssetLoader& operator=(const IAssetLoader& other) = default;
    IAssetLoader& operator=(IAssetLoader&& other) noexcept = default;

    _NODISCARD virtual asset_type_t GetHandlingAssetType() const = 0;
    _NODISCARD virtual XAssetInfoGeneric* LoadFromGlobalAssetPools(const std::string& assetName) const = 0;

    _NODISCARD virtual void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
    {
        // TODO: Make this pure virtual
        return nullptr;
    }

    _NODISCARD virtual bool CanLoadFromGdt() const
    {
        return false;
    }

    _NODISCARD virtual bool CanLoadFromRaw() const
    {
        return false;
    }

    virtual bool LoadFromGdt(const std::string& assetName, IGdtQueryable* gdtQueryable, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
    {
        return false;
    }

    virtual bool LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
    {
        return false;
    }

    virtual void FinalizeAssetsForZone(AssetLoadingContext* context) const
    {
        // Do nothing by default
    }
};
