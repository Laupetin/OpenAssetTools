#include "XModelCollMapDumperT4.h"

#include "Dumping/MapFile/MapFileDumper.h"
#include "Utils/Logging/Log.h"

#include <format>

using namespace T4;

namespace xmodel
{
    namespace
    {
        constexpr auto BRUSH_EDGE_MIN = 0u;
        constexpr auto BRUSH_EDGE_MAX = 1u;

        void WriteBrush(MapFileDumper& mapFileDumper, const BrushWrapper& brush)
        {
            if (brush.edgeCount[BRUSH_EDGE_MIN][0] != 0)
                mapFileDumper.WriteBrushPlane({
                    {1.0f, 0.0f, 0.0f},
                    brush.mins[0]
                });
            if (brush.edgeCount[BRUSH_EDGE_MAX][0] != 0)
                mapFileDumper.WriteBrushPlane({
                    {-1.0f, 0.0f, 0.0f},
                    -brush.maxs[0]
                });
            if (brush.edgeCount[BRUSH_EDGE_MIN][1] != 0)
                mapFileDumper.WriteBrushPlane({
                    {0.0f, 1.0f, 0.0f},
                    brush.mins[1]
                });
            if (brush.edgeCount[BRUSH_EDGE_MAX][1] != 0)
                mapFileDumper.WriteBrushPlane({
                    {0.0f, -1.0f, 0.0f},
                    -brush.maxs[1]
                });
            if (brush.edgeCount[BRUSH_EDGE_MIN][2] != 0)
                mapFileDumper.WriteBrushPlane({
                    {0.0f, 0.0f, 1.0f},
                    brush.mins[2]
                });
            if (brush.edgeCount[BRUSH_EDGE_MAX][2] != 0)
                mapFileDumper.WriteBrushPlane({
                    {0.0f, 0.0f, -1.0f},
                    -brush.maxs[2]
                });

            if (!brush.sides)
                return;

            for (auto sideIndex = 0u; sideIndex < brush.numsides; sideIndex++)
            {
                const auto* plane = brush.sides[sideIndex].plane;
                if (plane)
                    mapFileDumper.WriteBrushPlane({
                        {-plane->normal[0], -plane->normal[1], -plane->normal[2]},
                        -plane->dist
                    });
            }
        }

        bool WriteGeom(MapFileDumper& mapFileDumper, const PhysGeomInfo& geom)
        {
            // ParsePhysicsCollMap identifies brush geometry through this pointer without setting the type to PHYS_GEOM_BRUSH.
            if (geom.brush)
            {
                mapFileDumper.BeginBrush();
                WriteBrush(mapFileDumper, *geom.brush);
                mapFileDumper.EndBrush();
                return true;
            }

            switch (geom.type)
            {
            case PHYS_GEOM_BOX:
                mapFileDumper.BeginBrush();
                mapFileDumper.WritePhysicsBox({
                    {geom.offset[0],         geom.offset[1],         geom.offset[2]        },
                    {geom.halfLengths[0],    geom.halfLengths[1],    geom.halfLengths[2]   },
                    {geom.orientation[0][0], geom.orientation[0][1], geom.orientation[0][2]},
                    {geom.orientation[1][0], geom.orientation[1][1], geom.orientation[1][2]},
                    {geom.orientation[2][0], geom.orientation[2][1], geom.orientation[2][2]}
                });
                mapFileDumper.EndBrush();
                return true;

            case PHYS_GEOM_CYLINDER:
                mapFileDumper.BeginBrush();
                mapFileDumper.WritePhysicsCylinder({
                    {geom.offset[0],         geom.offset[1],         geom.offset[2]        },
                    geom.halfLengths[1],
                    geom.halfLengths[0] * 2.0f,
                    {geom.orientation[0][0], geom.orientation[0][1], geom.orientation[0][2]}
                });
                mapFileDumper.EndBrush();
                return true;

            default:
                return false;
            }
        }
    } // namespace

    void DumpXModelCollMapT4(const std::string& xmodelName, const AssetDumpingContext& context, const PhysGeomList* collMap)
    {
        if (!collMap || collMap->count == 0u || !collMap->geoms)
            return;

        const auto assetFile = context.OpenAssetFile(std::format("collmaps/{}.map", xmodelName));
        if (!assetFile)
            return;

        MapFileDumper mapFileDumper(*assetFile);
        mapFileDumper.Init();
        mapFileDumper.BeginEntity();
        mapFileDumper.WriteKeyValue("classname", "worldspawn");

        for (auto geomIndex = 0u; geomIndex < collMap->count; geomIndex++)
        {
            if (!WriteGeom(mapFileDumper, collMap->geoms[geomIndex]))
                con::warn(
                    "Cannot dump collmap geometry {} of xmodel \"{}\": unsupported or invalid type {}", geomIndex, xmodelName, collMap->geoms[geomIndex].type);
        }

        mapFileDumper.EndEntity();
    }
} // namespace xmodel
