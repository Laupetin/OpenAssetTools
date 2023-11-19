#pragma once

#include "AssetLoading/IAssetLoader.h"
#include "Game/IW5/IW5.h"
#include "IObjLoader.h"
#include "SearchPath/ISearchPath.h"

#include <map>
#include <memory>

namespace IW5
{
    class ObjLoader final : public IObjLoader
    {
        std::map<asset_type_t, std::unique_ptr<IAssetLoader>> m_asset_loaders_by_type;

        static void LoadImageFromIwi(GfxImage* image, ISearchPath* searchPath, Zone* zone);
        static void LoadImageFromLoadDef(GfxImage* image, Zone* zone);
        static void LoadImageData(ISearchPath* searchPath, Zone* zone);

        static bool IsMpZone(Zone* zone);
        static bool IsZmZone(Zone* zone);

    public:
        ObjLoader();

        bool SupportsZone(Zone* zone) const override;

        void LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone) const override;
        void UnloadContainersOfZone(Zone* zone) const override;

        void LoadObjDataForZone(ISearchPath* searchPath, Zone* zone) const override;

        bool LoadAssetForZone(AssetLoadingContext* context, asset_type_t assetType, const std::string& assetName) const override;
        void FinalizeAssetsForZone(AssetLoadingContext* context) const override;
    };
} // namespace IW5
