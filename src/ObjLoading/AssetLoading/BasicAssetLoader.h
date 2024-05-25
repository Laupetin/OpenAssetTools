#pragma once
#include "IAssetLoader.h"
#include "Pool/GlobalAssetPool.h"

template<typename T> class BasicAssetLoaderWithoutType : public IAssetLoader
{
public:
    _NODISCARD XAssetInfoGeneric* LoadFromGlobalAssetPools(const std::string& assetName) const override
    {
        return GlobalAssetPool<T>::GetAssetByName(assetName);
    }
};

template<typename AssetType> class BasicAssetLoader : public BasicAssetLoaderWithoutType<typename AssetType::Type>
{
public:
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

    _NODISCARD asset_type_t GetHandlingAssetType() const override
    {
        return AssetType::EnumEntry;
    }
};
