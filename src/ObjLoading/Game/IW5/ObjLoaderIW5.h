#pragma once

#include "IObjLoader.h"
#include "SearchPath/ISearchPath.h"

namespace IW5
{
    class ObjLoader final : public IObjLoader
    {
    public:
        void LoadReferencedContainersForZone(ISearchPath& searchPath, Zone& zone) const override;
        void UnloadContainersOfZone(Zone& zone) const override;

        void ConfigureCreatorCollection(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath, IGdtQueryable& gdt) const override;
    };
} // namespace IW5
