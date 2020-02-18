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
    asset_type_t m_type;

public:
    AssetPoolDynamic(const int priority, const asset_type_t type)
    {
        GlobalAssetPool<T>::LinkAssetPool(this, priority);
        m_type = type;
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
            delete entry->m_ptr;
            delete entry;
        }

        m_assets.clear();
        m_asset_lookup.clear();
    }

    XAssetInfo<T>* AddAsset(std::string name, T* asset, std::vector<std::string>& scriptStrings, std::vector<XAssetInfoGeneric*>& dependencies) override
    {
        auto* newInfo = new XAssetInfo<T>();
        newInfo->m_type = m_type;
        newInfo->m_name = std::move(name);
        newInfo->m_script_strings = std::move(scriptStrings);
        newInfo->m_dependencies = std::move(dependencies);

        T* newAsset = new T();
        memcpy(newAsset, asset, sizeof(T));
        newInfo->m_ptr = newAsset;
        
        m_assets.push_back(newInfo);
        m_asset_lookup[newInfo->m_name] = newInfo;
       
        GlobalAssetPool<T>::LinkAsset(this, newInfo);

        return newInfo;
    }
};
