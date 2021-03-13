#pragma once

#include <unordered_map>
#include <memory>

#include "IObjLoader.h"
#include "AssetLoading/IAssetLoader.h"
#include "SearchPath/ISearchPath.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class ObjLoader final : public IObjLoader
    {
        std::unordered_map<asset_type_t, std::unique_ptr<IAssetLoader>> m_asset_loaders_by_type;

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
    };
}
