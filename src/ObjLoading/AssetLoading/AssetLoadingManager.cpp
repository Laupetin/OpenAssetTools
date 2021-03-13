#include "AssetLoadingManager.h"
#include <iostream>

AssetLoadingManager::AssetLoadingManager(const std::unordered_map<asset_type_t, std::unique_ptr<IAssetLoader>>& assetLoadersByType, AssetLoadingContext& context):
    m_asset_loaders_by_type(assetLoadersByType),
    m_context(context),
    m_last_dependency_loaded(nullptr)
{
}

bool AssetLoadingManager::LoadAssetFromLoader(const asset_type_t assetType, const std::string& assetName)
{
    return LoadDependency(assetType, assetName) != nullptr;
}

void AssetLoadingManager::AddAsset(const asset_type_t assetType, const std::string& assetName, void* asset, std::vector<XAssetInfoGeneric*>& dependencies)
{
    m_last_dependency_loaded = m_context.m_zone->m_pools->AddAsset(assetType, assetName, asset, dependencies);
    if (m_last_dependency_loaded == nullptr)
        std::cout << "Failed to add asset of type \"" << m_context.m_zone->m_pools->GetAssetTypeName(assetType) << "\" to pool: \"" << assetName << "\"" << std::endl;
}

XAssetInfoGeneric* AssetLoadingManager::LoadIgnoredDependency(const asset_type_t assetType, const std::string& assetName, IAssetLoader* loader)
{
    auto* alreadyLoadedAsset = m_context.m_zone->m_pools->GetAsset(assetType, assetName);
    if (alreadyLoadedAsset)
        return alreadyLoadedAsset;

    auto* linkAsset = loader->CreateEmptyAsset(assetName, m_context.m_zone->GetMemory());
    if (linkAsset)
    {
        std::vector<XAssetInfoGeneric*> dependencies;
        AddAsset(assetType, assetName, linkAsset, dependencies);
        auto* lastDependency = m_last_dependency_loaded;
        m_last_dependency_loaded = nullptr;
        return lastDependency;
    }

    auto* existingAsset = loader->LoadFromGlobalAssetPools(assetName);
    if (existingAsset)
    {
        std::vector<XAssetInfoGeneric*> dependencies;
        AddAsset(existingAsset->m_type, existingAsset->m_name, existingAsset->m_ptr, dependencies);
        auto* lastDependency = m_last_dependency_loaded;
        m_last_dependency_loaded = nullptr;
        return lastDependency;
    }

    std::cout << "Failed to create empty asset for type \"" << m_context.m_zone->m_pools->GetAssetTypeName(assetType) << "\"" << std::endl;
    return nullptr;
}

XAssetInfoGeneric* AssetLoadingManager::LoadAssetDependency(const asset_type_t assetType, const std::string& assetName, IAssetLoader* loader)
{
    if (loader->CanLoadFromGdt() && loader->LoadFromGdt(assetName, &m_context, m_context.m_zone->GetMemory(), this))
    {
        auto* lastDependency = m_last_dependency_loaded;
        m_last_dependency_loaded = nullptr;
        return lastDependency;
    }

    if (loader->CanLoadFromRaw() && loader->LoadFromRaw(assetName, m_context.m_raw_search_path, m_context.m_zone->GetMemory(), this))
    {
        auto* lastDependency = m_last_dependency_loaded;
        m_last_dependency_loaded = nullptr;
        return lastDependency;
    }

    auto* existingAsset = loader->LoadFromGlobalAssetPools(assetName);
    if (existingAsset)
    {
        std::vector<XAssetInfoGeneric*> dependencies;
        for (const auto* dependency : existingAsset->m_dependencies)
        {
            auto* newDependency = LoadDependency(dependency->m_type, dependency->m_name);
            if (newDependency)
                dependencies.push_back(newDependency);
            else
                return nullptr;
        }

        AddAsset(existingAsset->m_type, existingAsset->m_name, existingAsset->m_ptr, dependencies);
        auto* lastDependency = m_last_dependency_loaded;
        m_last_dependency_loaded = nullptr;
        return lastDependency;
    }

    std::cout << "Failed to load asset of type \"" << m_context.m_zone->m_pools->GetAssetTypeName(assetType) << "\": \"" << assetName << "\"" << std::endl;
    return nullptr;
}

XAssetInfoGeneric* AssetLoadingManager::LoadDependency(const asset_type_t assetType, const std::string& assetName)
{
    auto* alreadyLoadedAsset = m_context.m_zone->m_pools->GetAsset(assetType, assetName);
    if (alreadyLoadedAsset)
        return alreadyLoadedAsset;

    const auto loader = m_asset_loaders_by_type.find(assetType);
    if (loader != m_asset_loaders_by_type.end())
    {
        const auto ignoreEntry = m_context.m_ignored_asset_map.find(assetName);
        if(ignoreEntry != m_context.m_ignored_asset_map.end() && ignoreEntry->second == assetType)
        {
            const auto linkAssetName = ',' + assetName;

            return LoadIgnoredDependency(assetType, linkAssetName, loader->second.get());
        }

        return LoadAssetDependency(assetType, assetName, loader->second.get());
    }

    std::cout << "Failed to find loader for asset type \"" << m_context.m_zone->m_pools->GetAssetTypeName(assetType) << "\"" << std::endl;
    return nullptr;
}
