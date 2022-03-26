#pragma once

#include "Game/IW4/IW4.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "SearchPath/ISearchPath.h"
#include "Techset/TechsetDefinition.h"

namespace IW4
{
    class AssetLoaderTechniqueSet final : public BasicAssetLoader<ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet>
    {
        static std::string GetTechniqueFileName(const std::string& techniqueName);
        static std::string GetTechsetFileName(const std::string& techsetAssetName);
        static MaterialTechnique* LoadTechniqueWithName(const std::string& techniqueName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager);
        static bool CreateTechsetFromDefinition(const std::string& assetName, const techset::TechsetDefinition& definition, ISearchPath* searchPath, MemoryManager* memory,
                                                IAssetLoadingManager* manager);

    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
}
