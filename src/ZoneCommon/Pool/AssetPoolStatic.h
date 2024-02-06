#pragma once

#include "AssetPool.h"
#include "GlobalAssetPool.h"
#include "XAssetInfo.h"

#include <cstring>
#include <stdexcept>

template<typename T> class AssetPoolStatic final : public AssetPool<T>
{
    using AssetPool<T>::m_asset_lookup;

    struct AssetPoolEntry
    {
        XAssetInfo<T>* m_info;

        union
        {
            T m_entry;
            AssetPoolEntry* m_next;
        };
    };

    AssetPoolEntry* m_free;
    AssetPoolEntry* m_pool;
    XAssetInfo<T>* m_info_pool;
    size_t m_capacity;
    asset_type_t m_type;

public:
    AssetPoolStatic(const size_t capacity, const int priority, const asset_type_t type)
    {
        m_capacity = capacity;
        m_type = type;

        if (m_capacity > 0)
        {
            m_pool = new AssetPoolEntry[m_capacity];
            m_info_pool = new XAssetInfo<T>[m_capacity];

            for (size_t i = 0; i < m_capacity - 1; i++)
            {
                m_pool[i].m_info = &m_info_pool[i];
                m_pool[i].m_next = &m_pool[i + 1];
            }
            m_pool[m_capacity - 1].m_info = &m_info_pool[m_capacity - 1];
            m_pool[m_capacity - 1].m_next = nullptr;

            m_free = m_pool;

            GlobalAssetPool<T>::LinkAssetPool(this, priority);
        }
        else
        {
            m_pool = nullptr;
            m_free = nullptr;
            m_info_pool = nullptr;
        }
    }

    AssetPoolStatic(AssetPoolStatic<T>&) = delete;
    AssetPoolStatic(AssetPoolStatic<T>&&) = delete;
    AssetPoolStatic<T>& operator=(AssetPoolStatic<T>&) = delete;
    AssetPoolStatic<T>& operator=(AssetPoolStatic<T>&&) = default;

    ~AssetPoolStatic() override
    {
        if (m_capacity > 0)
            GlobalAssetPool<T>::UnlinkAssetPool(this);

        delete[] m_pool;
        m_pool = nullptr;

        delete[] m_info_pool;
        m_info_pool = nullptr;

        m_free = nullptr;
        m_capacity = 0;
    }

    XAssetInfo<T>* AddAsset(std::unique_ptr<XAssetInfo<T>> xAssetInfo) override
    {
        if (m_free == nullptr)
            throw std::runtime_error("Could not add asset to static asset pool: capacity exhausted.");

        AssetPoolEntry* poolSlot = m_free;
        m_free = m_free->m_next;

        const T* pAsset = xAssetInfo->Asset();
        xAssetInfo->m_ptr = static_cast<void*>(&poolSlot->m_entry);
        memcpy(&poolSlot->m_entry, pAsset, sizeof(T));

        *poolSlot->m_info = std::move(*xAssetInfo);

        m_asset_lookup[poolSlot->m_info->m_name] = poolSlot->m_info;

        GlobalAssetPool<T>::LinkAsset(this, poolSlot->m_info);

        return poolSlot->m_info;
    }
};
