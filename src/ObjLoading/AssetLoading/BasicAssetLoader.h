#pragma once
#include "IAssetLoader.h"
#include "Pool/GlobalAssetPool.h"

template<asset_type_t AssetType, typename T>
class BasicAssetLoader : public IAssetLoader
{
public:
    _NODISCARD asset_type_t GetHandlingAssetType() const override
    {
        return AssetType;
    }

    _NODISCARD XAssetInfoGeneric* LoadFromGlobalAssetPools(const std::string& assetName) const override
    {
        return GlobalAssetPool<T>::GetAssetByName(assetName);
    }
};
