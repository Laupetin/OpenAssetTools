#include "AssetLoadingManager.h"

#include <iostream>

AssetLoadingManager::AssetLoadingManager(const std::map<asset_type_t, std::unique_ptr<IAssetLoader>>& assetLoadersByType, AssetLoadingContext& context)
    : m_asset_loaders_by_type(assetLoadersByType),
      m_context(context),
      m_last_dependency_loaded(nullptr)
{
}

bool AssetLoadingManager::LoadAssetFromLoader(const asset_type_t assetType, const std::string& assetName)
{
    return LoadDependency(assetType, assetName) != nullptr;
}

AssetLoadingContext* AssetLoadingManager::GetAssetLoadingContext() const
{
    return &m_context;
}

XAssetInfoGeneric* AssetLoadingManager::AddAssetInternal(std::unique_ptr<XAssetInfoGeneric> xAssetInfo)
{
    const auto assetType = xAssetInfo->m_type;
    const auto* pAssetName = xAssetInfo->m_name.c_str();

    m_last_dependency_loaded = m_context.m_zone->m_pools->AddAsset(std::move(xAssetInfo));
    if (m_last_dependency_loaded == nullptr)
        std::cout << "Failed to add asset of type \"" << m_context.m_zone->m_pools->GetAssetTypeName(assetType) << "\" to pool: \"" << pAssetName << "\"\n";

    return m_last_dependency_loaded;
}

XAssetInfoGeneric* AssetLoadingManager::AddAsset(const asset_type_t assetType,
                                                 const std::string& assetName,
                                                 void* asset,
                                                 std::vector<XAssetInfoGeneric*> dependencies,
                                                 std::vector<scr_string_t> usedScriptStrings)
{
    return AddAsset(std::make_unique<XAssetInfoGeneric>(assetType, assetName, asset, std::move(dependencies), std::move(usedScriptStrings)));
}

XAssetInfoGeneric* AssetLoadingManager::AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo)
{
    xAssetInfo->m_zone = m_context.m_zone;
    return AddAssetInternal(std::move(xAssetInfo));
}

XAssetInfoGeneric* AssetLoadingManager::LoadIgnoredDependency(const asset_type_t assetType, const std::string& assetName, IAssetLoader* loader)
{
    auto* alreadyLoadedAsset = m_context.m_zone->m_pools->GetAsset(assetType, assetName);
    if (alreadyLoadedAsset)
        return alreadyLoadedAsset;

    auto* linkAsset = loader->CreateEmptyAsset(assetName, m_context.m_zone->GetMemory());
    if (linkAsset)
    {
        AddAsset(assetType, assetName, linkAsset, std::vector<XAssetInfoGeneric*>(), std::vector<scr_string_t>());
        auto* lastDependency = m_last_dependency_loaded;
        m_last_dependency_loaded = nullptr;
        return lastDependency;
    }

    auto* existingAsset = loader->LoadFromGlobalAssetPools(assetName);
    if (existingAsset)
    {
        AddAssetInternal(std::make_unique<XAssetInfoGeneric>(*existingAsset));
        auto* lastDependency = m_last_dependency_loaded;
        m_last_dependency_loaded = nullptr;
        return lastDependency;
    }

    std::cout << "Failed to create empty asset \"" << assetName << "\" for type \"" << m_context.m_zone->m_pools->GetAssetTypeName(assetType) << "\"\n";
    return nullptr;
}

XAssetInfoGeneric* AssetLoadingManager::LoadAssetDependency(const asset_type_t assetType, const std::string& assetName, IAssetLoader* loader)
{
    if (loader->CanLoadFromGdt() && !m_context.m_gdt_files.empty()
        && loader->LoadFromGdt(assetName, &m_context, m_context.m_zone->GetMemory(), this, m_context.m_zone))
    {
        auto* lastDependency = m_last_dependency_loaded;
        m_last_dependency_loaded = nullptr;
        return lastDependency;
    }

    if (loader->CanLoadFromRaw() && loader->LoadFromRaw(assetName, m_context.m_raw_search_path, m_context.m_zone->GetMemory(), this, m_context.m_zone))
    {
        auto* lastDependency = m_last_dependency_loaded;
        m_last_dependency_loaded = nullptr;
        return lastDependency;
    }

    auto* existingAsset = loader->LoadFromGlobalAssetPools(assetName);
    if (!existingAsset && !assetName.empty() && assetName[0] != ',')
        existingAsset = loader->LoadFromGlobalAssetPools(',' + assetName);

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

        // Make sure any used script string is available in the created zone
        // The replacement of the scr_string_t values will be done upon writing
        for (const auto scrString : existingAsset->m_used_script_strings)
            m_context.m_zone->m_script_strings.AddOrGetScriptString(existingAsset->m_zone->m_script_strings.CValue(scrString));

        AddAssetInternal(std::make_unique<XAssetInfoGeneric>(existingAsset->m_type,
                                                             existingAsset->m_name,
                                                             existingAsset->m_ptr,
                                                             std::move(dependencies),
                                                             existingAsset->m_used_script_strings,
                                                             existingAsset->m_indirect_asset_references,
                                                             existingAsset->m_zone));

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
        if (ignoreEntry != m_context.m_ignored_asset_map.end() && ignoreEntry->second == assetType)
        {
            const auto linkAssetName = ',' + assetName;

            return LoadIgnoredDependency(assetType, linkAssetName, loader->second.get());
        }

        return LoadAssetDependency(assetType, assetName, loader->second.get());
    }

    std::cout << "Failed to find loader for asset type \"" << m_context.m_zone->m_pools->GetAssetTypeName(assetType) << "\"" << std::endl;
    return nullptr;
}
