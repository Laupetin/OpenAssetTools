#include "PhysCollmapDumperIW4.h"

#include "Maps/MapFileDumper.h"
#include "PhysCollmap/PhysCollmapCommon.h"

#include <cassert>
#include <sstream>

using namespace IW4;

namespace phys_collmap
{
    bool DumperIW4::ShouldDump(XAssetInfo<PhysCollmap>* asset)
    {
        return true;
    }

    void DumperIW4::DumpAsset(AssetDumpingContext& context, XAssetInfo<PhysCollmap>* asset)
    {
        const auto* physCollmap = asset->Asset();
        const auto assetFile = context.OpenAssetFile(phys_collmap::GetFileNameForAssetName(asset->m_name));

        if (!assetFile)
            return;

        MapFileDumper mapFileDumper(*assetFile);
        mapFileDumper.Init();

        if (physCollmap->count <= 0 || physCollmap->geoms == nullptr)
            return;

        mapFileDumper.BeginEntity();

        mapFileDumper.WriteKeyValue("classname", "worldspawn");

        for (auto i = 0u; i < physCollmap->count; i++)
        {
            const auto& geom = physCollmap->geoms[i];
            mapFileDumper.BeginBrush();

            switch (geom.type)
            {
            case PHYS_GEOM_NONE:
                // TODO: Dump BrushWrapper (probably GJK related)
                mapFileDumper.WriteComment("TODO: Brush data");
                break;
            case PHYS_GEOM_BOX:
                mapFileDumper.WritePhysicsBox({
                    {geom.bounds.midPoint.v[0], geom.bounds.midPoint.v[1], geom.bounds.midPoint.v[2]},
                    {geom.bounds.halfSize.v[0], geom.bounds.halfSize.v[1], geom.bounds.halfSize.v[2]},
                    {geom.orientation[0][0],    geom.orientation[0][1],    geom.orientation[0][2]   },
                    {geom.orientation[1][0],    geom.orientation[1][1],    geom.orientation[1][2]   },
                    {geom.orientation[2][0],    geom.orientation[2][1],    geom.orientation[2][2]   }
                });
                break;

            case PHYS_GEOM_CYLINDER:
                mapFileDumper.WritePhysicsCylinder({
                    {geom.bounds.midPoint.v[0], geom.bounds.midPoint.v[1], geom.bounds.midPoint.v[2]},
                    geom.bounds.halfSize.v[0],
                    geom.bounds.halfSize.v[2] * 2,
                    {geom.orientation[0][0],    geom.orientation[0][1],    geom.orientation[0][2]   }
                });
                break;

            case PHYS_GEOM_BRUSHMODEL:
            case PHYS_GEOM_BRUSH:
            case PHYS_GEOM_COLLMAP:
            case PHYS_GEOM_CAPSULE:
            case PHYS_GEOM_GLASS:
            default:
                // These do not seem to appear inside any collmap assets
                assert(false);
                break;
            }

            mapFileDumper.EndBrush();
        }

        mapFileDumper.EndEntity();
    }
} // namespace phys_collmap
