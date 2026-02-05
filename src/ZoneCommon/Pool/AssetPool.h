#pragma once

#include "Game/IAsset.h"
#include "Zone/ZoneTypes.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Zone;
template<typename T> class XAssetInfo;
class XAssetInfoGeneric;
class IndirectAssetReference;

class AssetPool
{
public:
    class Iterator
    {
    public:
        explicit Iterator(std::unordered_map<std::string, XAssetInfoGeneric*>::iterator i);

        bool operator!=(const Iterator& rhs) const;
        XAssetInfoGeneric* operator*() const;
        void operator++();

    private:
        std::unordered_map<std::string, XAssetInfoGeneric*>::iterator m_iterator;
    };

    class CIterator
    {
    public:
        explicit CIterator(std::unordered_map<std::string, XAssetInfoGeneric*>::const_iterator i);

        bool operator!=(const CIterator& rhs) const;
        const XAssetInfoGeneric* operator*() const;
        void operator++();

    private:
        std::unordered_map<std::string, XAssetInfoGeneric*>::const_iterator m_iterator;
    };

    AssetPool() = default;
    ~AssetPool() = default;

    AssetPool(AssetPool&) = delete;
    AssetPool(AssetPool&&) = delete;
    AssetPool& operator=(AssetPool&) = delete;
    AssetPool& operator=(AssetPool&&) = delete;

    XAssetInfoGeneric* AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo);
    XAssetInfoGeneric* GetAsset(const std::string& name);

    [[nodiscard]] size_t size() const;
    [[nodiscard]] bool empty() const;

    [[nodiscard]] Iterator begin();
    [[nodiscard]] Iterator end();

    [[nodiscard]] CIterator begin() const;
    [[nodiscard]] CIterator end() const;

private:
    std::unordered_map<std::string, XAssetInfoGeneric*> m_asset_lookup;
    std::vector<std::unique_ptr<XAssetInfoGeneric>> m_assets;
};

template<AssetDefinition Asset_t> class AssetPoolIterator
{
public:
    explicit AssetPoolIterator(AssetPool::Iterator i)
        : m_iterator(std::move(i))
    {
    }

    bool operator!=(const AssetPoolIterator& rhs) const
    {
        return m_iterator != rhs.m_iterator;
    }

    XAssetInfo<typename Asset_t::Type>* operator*() const
    {
        return reinterpret_cast<XAssetInfo<typename Asset_t::Type>*>(*m_iterator);
    }

    void operator++()
    {
        ++m_iterator;
    }

private:
    AssetPool::Iterator m_iterator;
};

template<AssetDefinition Asset_t> class AssetPoolIterators
{
public:
    explicit AssetPoolIterators(AssetPool& assetPool)
        : m_asset_pool(assetPool)
    {
    }

    AssetPoolIterator<Asset_t> begin()
    {
        return AssetPoolIterator<Asset_t>(m_asset_pool.begin());
    }

    AssetPoolIterator<Asset_t> end()
    {
        return AssetPoolIterator<Asset_t>(m_asset_pool.end());
    }

    [[nodiscard]] size_t size() const
    {
        return m_asset_pool.size();
    }

    [[nodiscard]] bool empty() const
    {
        return m_asset_pool.empty();
    }

private:
    AssetPool& m_asset_pool;
};

class ZoneAssetPools
{
public:
    using all_iterator = std::vector<XAssetInfoGeneric*>::const_iterator;

    ZoneAssetPools(Zone& zone, zone_priority_t priority);
    ~ZoneAssetPools();
    ZoneAssetPools(const ZoneAssetPools& other) = delete;
    ZoneAssetPools(ZoneAssetPools&& other) noexcept = delete;
    ZoneAssetPools& operator=(const ZoneAssetPools& other) = delete;
    ZoneAssetPools& operator=(ZoneAssetPools&& other) noexcept = delete;

    XAssetInfoGeneric* AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo);
    XAssetInfoGeneric* AddAsset(asset_type_t type,
                                std::string name,
                                void* asset,
                                std::vector<XAssetInfoGeneric*> dependencies,
                                std::vector<scr_string_t> usedScriptStrings,
                                std::vector<IndirectAssetReference> indirectAssetReferences);
    [[nodiscard]] XAssetInfoGeneric* GetAsset(asset_type_t type, const std::string& name) const;
    [[nodiscard]] XAssetInfoGeneric* GetAssetOrAssetReference(asset_type_t type, const std::string& name) const;

    template<AssetDefinition Asset_t> XAssetInfo<typename Asset_t::Type>* AddAsset(std::unique_ptr<XAssetInfo<typename Asset_t::Type>> xAssetInfo)
    {
        return reinterpret_cast<XAssetInfo<typename Asset_t::Type>*>(AddAsset(std::unique_ptr<XAssetInfoGeneric>(xAssetInfo.release())));
    }

    template<AssetDefinition Asset_t> [[nodiscard]] XAssetInfo<typename Asset_t::Type>* GetAsset(const std::string& name) const
    {
        return reinterpret_cast<XAssetInfo<typename Asset_t::Type>*>(GetAsset(Asset_t::EnumEntry, name));
    }

    template<AssetDefinition Asset_t> [[nodiscard]] XAssetInfo<typename Asset_t::Type>* GetAssetOrAssetReference(const std::string& name) const
    {
        return reinterpret_cast<XAssetInfo<typename Asset_t::Type>*>(GetAssetOrAssetReference(Asset_t::EnumEntry, name));
    }

    template<AssetDefinition Asset_t> [[nodiscard]] AssetPoolIterators<Asset_t> PoolAssets() const
    {
        return AssetPoolIterators<Asset_t>(*m_asset_pools[Asset_t::EnumEntry]);
    }

    [[nodiscard]] size_t GetTotalAssetCount() const;

    [[nodiscard]] all_iterator begin() const;
    [[nodiscard]] all_iterator end() const;

private:
    Zone& m_zone;
    std::vector<std::unique_ptr<AssetPool>> m_asset_pools;
    std::vector<XAssetInfoGeneric*> m_assets_in_order;
};

#include "XAssetInfo.h"
#include "Zone/Zone.h"
