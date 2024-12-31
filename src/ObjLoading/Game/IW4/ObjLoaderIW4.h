#pragma once

#include "IObjLoader.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/Zone.h"

namespace IW4
{
    class ObjLoader final : public IObjLoader
    {
    public:
        void LoadReferencedContainersForZone(ISearchPath& searchPath, Zone& zone) const override;
        void UnloadContainersOfZone(Zone& zone) const override;

        void ConfigureCreatorCollection(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath, IGdtQueryable& gdt) const override;
    };
} // namespace IW4
