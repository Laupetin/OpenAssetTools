#pragma once

#include "AssetPool.h"
#include "GlobalAssetPool.h"
#include "XAssetInfo.h"

#include <cstring>

template <typename T>
class AssetPoolDynamic final : public AssetPool<T>
{
    using AssetPool<T>::m_asset_lookup;

    std::vector<XAssetInfo<T>*> m_assets;

public:
    explicit AssetPoolDynamic(const int priority)
    {
        GlobalAssetPool<T>::LinkAssetPool(this, priority);
    }

    AssetPoolDynamic(AssetPoolDynamic<T>&) = delete;
    AssetPoolDynamic(AssetPoolDynamic<T>&&) = delete;
    AssetPoolDynamic<T>& operator =(AssetPoolDynamic<T>&) = delete;
    AssetPoolDynamic<T>& operator =(AssetPoolDynamic<T>&&) = default;

    ~AssetPoolDynamic() override
    {
        GlobalAssetPool<T>::UnlinkAssetPool(this);

        for(auto* entry : m_assets)
        {
            delete entry->m_asset;
            delete entry;
        }

        m_assets.clear();
        m_asset_lookup.clear();
    }

    XAssetInfo<T>* AddAsset(std::string name, T* asset, std::vector<std::string>& scriptStrings, std::vector<XAssetDependency>& dependencies) override
    {
        auto* newInfo = new XAssetInfo<T>();
        newInfo->m_name = std::move(name);
        newInfo->m_script_strings = std::move(scriptStrings);
        newInfo->m_dependencies = std::move(dependencies);

        T* newAsset = new T();
        memcpy(newAsset, asset, sizeof(T));
        newInfo->m_asset = newAsset;
        
        m_assets.push_back(newInfo);
        m_asset_lookup[newInfo->m_name] = newInfo;
       
        GlobalAssetPool<T>::LinkAsset(this, newInfo);

        return newInfo;
    }
};
