#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "StateMap/StateMapDefinition.h"
#include "Techset/TechniqueStateMapCache.h"
#include "Techset/TechsetDefinition.h"
#include "Techset/TechsetDefinitionCache.h"

namespace IW4
{
    class AssetLoaderTechniqueSet final : public BasicAssetLoader<AssetTechniqueSet>
    {
        static bool CreateTechsetFromDefinition(const std::string& assetName,
                                                const techset::TechsetDefinition& definition,
                                                ISearchPath* searchPath,
                                                MemoryManager* memory,
                                                IAssetLoadingManager* manager);

    public:
        static std::string GetTechsetFileName(const std::string& techsetAssetName);
        static std::string GetTechniqueFileName(const std::string& techniqueName);
        static std::string GetStateMapFileName(const std::string& stateMapName);

        static techset::TechsetDefinition*
            LoadTechsetDefinition(const std::string& assetName, ISearchPath* searchPath, techset::TechsetDefinitionCache* definitionCache);
        static const state_map::StateMapDefinition*
            LoadStateMapDefinition(const std::string& stateMapName, ISearchPath* searchPath, techset::TechniqueStateMapCache* stateMapCache);

        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool
            LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
} // namespace IW4
