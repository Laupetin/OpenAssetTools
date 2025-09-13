#include "XModelCollmapDumperT6.h"

#include "Maps/MapFileDumper.h"
#include "XModel/XModelCommon.h"

#include <cassert>

using namespace T6;

namespace xmodel
{
    void DumpCollMap(const std::string& collMapName, const AssetDumpingContext& context, const Collmap& collMap)
    {
        if (!collMap.geomList)
            return;

        const auto& geomList = *collMap.geomList;

        const auto assetFile = context.OpenAssetFile(GetFileNameForCollMap(collMapName));
        if (!assetFile)
            return;

        MapFileDumper mapFileDumper(*assetFile);
        mapFileDumper.Init();

        if (geomList.count <= 0 || !geomList.geoms)
            return;

        mapFileDumper.BeginEntity();

        mapFileDumper.WriteKeyValue("classname", "worldspawn");

        for (auto i = 0u; i < geomList.count; i++)
        {
            const auto& geom = geomList.geoms[i];
            mapFileDumper.BeginBrush();

            switch (geom.type)
            {
            case PHYS_GEOM_BOX:
                mapFileDumper.WritePhysicsBox({
                    {geom.offset.x,         geom.offset.y,         geom.offset.z        },
                    {geom.halfLengths.x,    geom.halfLengths.y,    geom.halfLengths.z   },
                    {geom.orientation[0].x, geom.orientation[0].y, geom.orientation[0].z},
                    {geom.orientation[1].x, geom.orientation[1].y, geom.orientation[1].z},
                    {geom.orientation[2].x, geom.orientation[2].y, geom.orientation[2].z}
                });
                break;

            case PHYS_GEOM_BRUSH:
                // TODO
                mapFileDumper.WriteComment("TODO: Brush data");
                break;

            case PHYS_GEOM_CYLINDER:
                mapFileDumper.WritePhysicsCylinder({
                    {geom.offset.x,         geom.offset.y,         geom.offset.z        },
                    geom.halfLengths.x,
                    geom.halfLengths.z * 2,
                    {geom.orientation[0].x, geom.orientation[0].y, geom.orientation[0].z}
                });
                break;

            case PHYS_GEOM_NONE:
            case PHYS_GEOM_CYLINDER_LARGE:
            case PHYS_GEOM_CAPSULE:
            case PHYS_GEOM_POINT:

            default:
                // These do not seem to appear inside any coll maps
                assert(false);
                break;
            }

            mapFileDumper.EndBrush();
        }

        mapFileDumper.EndEntity();
    }
} // namespace xmodel
