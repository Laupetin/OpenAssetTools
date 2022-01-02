#pragma once
#include "IAssetLoader.h"
#include "Pool/GlobalAssetPool.h"

template<typename T>
class BasicAssetLoaderWithoutType : public IAssetLoader
{
public:
    _NODISCARD XAssetInfoGeneric* LoadFromGlobalAssetPools(const std::string& assetName) const override
    {
        return GlobalAssetPool<T>::GetAssetByName(assetName);
    }
};

template<asset_type_t AssetType, typename T>
class BasicAssetLoader : public BasicAssetLoaderWithoutType<T>
{
public:
    _NODISCARD asset_type_t GetHandlingAssetType() const override
    {
        return AssetType;
    }
};
