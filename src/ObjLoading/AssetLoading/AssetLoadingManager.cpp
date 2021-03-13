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

XAssetInfoGeneric* AssetLoadingManager::LoadDependency(const asset_type_t assetType, const std::string& assetName)
{
    auto* existingAsset = m_context.m_zone->m_pools->GetAsset(assetType, assetName);
    if (existingAsset)
        return existingAsset;

    const auto loader = m_asset_loaders_by_type.find(assetType);
    if (loader != m_asset_loaders_by_type.end())
    {
        if (loader->second->CanLoadFromGdt() && loader->second->LoadFromGdt(assetName, &m_context, m_context.m_zone->GetMemory(), this))
        {
            auto* lastDependency = m_last_dependency_loaded;
            m_last_dependency_loaded = nullptr;
            return lastDependency;
        }

        if (loader->second->CanLoadFromRaw() && loader->second->LoadFromRaw(assetName, m_context.m_raw_search_path, m_context.m_zone->GetMemory(), this))
        {
            auto* lastDependency = m_last_dependency_loaded;
            m_last_dependency_loaded = nullptr;
            return lastDependency;
        }

        auto* existingAsset = loader->second->LoadFromGlobalAssetPools(assetName);
        if(existingAsset)
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
    }
    else
    {
        std::cout << "Failed to find loader for asset type \"" << m_context.m_zone->m_pools->GetAssetTypeName(assetType) << "\"" << std::endl;
    }

    return nullptr;
}
