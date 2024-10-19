#pragma once

#include "AssetLoading/IAssetLoader.h"
#include "IObjLoader.h"
#include "SearchPath/ISearchPath.h"

#include <memory>
#include <unordered_map>

namespace T5
{
    class ObjLoader final : public IObjLoader
    {
    public:
        ObjLoader();

        void LoadReferencedContainersForZone(ISearchPath& searchPath, Zone& zone) const override;
        void UnloadContainersOfZone(Zone& zone) const override;

        bool LoadAssetForZone(AssetLoadingContext& context, asset_type_t assetType, const std::string& assetName) const override;
        void FinalizeAssetsForZone(AssetLoadingContext& context) const override;

    private:
        static bool IsMpZone(const Zone& zone);
        static bool IsZmZone(const Zone& zone);

        std::unordered_map<asset_type_t, std::unique_ptr<IAssetLoader>> m_asset_loaders_by_type;
    };
} // namespace T5
