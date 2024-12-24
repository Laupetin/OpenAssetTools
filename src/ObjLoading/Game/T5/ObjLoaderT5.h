#pragma once

#include "AssetLoading/IAssetLoader.h"
#include "IObjLoader.h"
#include "SearchPath/ISearchPath.h"

namespace T5
{
    class ObjLoader final : public IObjLoader
    {
    public:
        void LoadReferencedContainersForZone(ISearchPath& searchPath, Zone& zone) const override;
        void UnloadContainersOfZone(Zone& zone) const override;

        void ConfigureCreatorCollection(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath) const override;
    };
} // namespace T5
