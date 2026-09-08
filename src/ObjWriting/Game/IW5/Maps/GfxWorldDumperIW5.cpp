#include "GfxWorldDumperIW5.h"

#include "Json/JsonCommon.h"
#include "Maps/GfxWorldCommon.h"

#include <iomanip>
#include <nlohmann/json.hpp>
#include <string>

using namespace nlohmann;
using namespace IW5;

namespace
{
    // A surface addresses the shared vertex/index arrays rather than owning geometry:
    // for t in [0, triCount): indices[baseIndex + t*3 + i] + firstVertex
    void CreateJsonSurfaces(json& jRoot, const GfxWorld& gfxWorld)
    {
        auto jSurfaces = json::array();

        if (gfxWorld.dpvs.surfaces)
        {
            for (auto surfaceIndex = 0u; surfaceIndex < gfxWorld.surfaceCount; surfaceIndex++)
            {
                const auto& surface = gfxWorld.dpvs.surfaces[surfaceIndex];

                json jSurface{
                    {"firstVertex", surface.tris.firstVertex},
                    {"vertexCount", surface.tris.vertexCount},
                    {"triCount",    surface.tris.triCount   },
                    {"baseIndex",   surface.tris.baseIndex  },
                };

                if (surface.material && surface.material->info.name)
                    jSurface["material"] = surface.material->info.name;

                jSurfaces.emplace_back(std::move(jSurface));
            }
        }

        jRoot["surfaces"] = std::move(jSurfaces);
    }

    // The scenery props. The MapEnts entity string holds spawns, script models and
    // pathnodes, but not these.
    void CreateJsonStaticModels(json& jRoot, const GfxWorld& gfxWorld)
    {
        auto jStaticModels = json::array();

        if (gfxWorld.dpvs.smodelDrawInsts)
        {
            for (auto smodelIndex = 0u; smodelIndex < gfxWorld.dpvs.smodelCount; smodelIndex++)
            {
                const auto& smodel = gfxWorld.dpvs.smodelDrawInsts[smodelIndex];
                const auto& placement = smodel.placement;

                json jStaticModel{
                    {"origin", JsonVec3{placement.origin[0], placement.origin[1], placement.origin[2]}},
                    {"axis",
                     json::array({
                         json::array({placement.axis[0][0], placement.axis[0][1], placement.axis[0][2]}),
                         json::array({placement.axis[1][0], placement.axis[1][1], placement.axis[1][2]}),
                         json::array({placement.axis[2][0], placement.axis[2][1], placement.axis[2][2]}),
                     })                                                                               },
                    {"scale", placement.scale                                                         },
                };

                if (smodel.model && smodel.model->name)
                    jStaticModel["model"] = smodel.model->name;

                jStaticModels.emplace_back(std::move(jStaticModel));
            }
        }

        jRoot["staticModels"] = std::move(jStaticModels);
    }

    void WriteManifest(std::ostream& stream, const GfxWorld& gfxWorld, const std::string& assetName)
    {
        json jRoot;

        jRoot["_type"] = "gfxworld";
        jRoot["_version"] = 1;
        jRoot["_game"] = "iw5";

        jRoot["name"] = assetName;
        if (gfxWorld.baseName)
            jRoot["baseName"] = gfxWorld.baseName;
        jRoot["checksum"] = gfxWorld.checksum;

        // Bounds are stored as a midpoint plus a half size; emit the corners.
        const auto& mid = gfxWorld.bounds.midPoint.v;
        const auto& half = gfxWorld.bounds.halfSize.v;
        jRoot["bounds"] = {
            {"mins", JsonVec3{mid[0] - half[0], mid[1] - half[1], mid[2] - half[2]}},
            {"maxs", JsonVec3{mid[0] + half[0], mid[1] + half[1], mid[2] + half[2]}},
        };

        jRoot["vertexCount"] = gfxWorld.draw.vertexCount;
        jRoot["vertexStride"] = static_cast<unsigned>(sizeof(GfxWorldVertex));
        jRoot["vertexFile"] = gfx_world::GetVertexFileNameForAssetName(assetName);
        jRoot["indexCount"] = gfxWorld.draw.indexCount;
        jRoot["indexStride"] = static_cast<unsigned>(sizeof(r_index_t));
        jRoot["indexFile"] = gfx_world::GetIndexFileNameForAssetName(assetName);

        jRoot["surfaceCount"] = gfxWorld.surfaceCount;
        jRoot["staticModelCount"] = gfxWorld.dpvs.smodelCount;

        CreateJsonSurfaces(jRoot, gfxWorld);
        CreateJsonStaticModels(jRoot, gfxWorld);

        stream << std::setw(4) << jRoot << "\n";
    }
} // namespace

namespace gfx_world
{
    void DumperIW5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetGfxWorld::Type>& asset)
    {
        const auto* gfxWorld = asset.Asset();

        const auto manifestFile = context.OpenAssetFile(GetJsonFileNameForAssetName(asset.m_name));
        if (!manifestFile)
            return;

        WriteManifest(*manifestFile, *gfxWorld, asset.m_name);

        if (gfxWorld->draw.vd.vertices && gfxWorld->draw.vertexCount > 0)
        {
            if (const auto vertexFile = context.OpenAssetFile(GetVertexFileNameForAssetName(asset.m_name)))
            {
                vertexFile->write(reinterpret_cast<const char*>(gfxWorld->draw.vd.vertices),
                                  static_cast<std::streamsize>(sizeof(GfxWorldVertex)) * gfxWorld->draw.vertexCount);
            }
        }

        if (gfxWorld->draw.indices && gfxWorld->draw.indexCount > 0)
        {
            if (const auto indexFile = context.OpenAssetFile(GetIndexFileNameForAssetName(asset.m_name)))
            {
                indexFile->write(reinterpret_cast<const char*>(gfxWorld->draw.indices),
                                 static_cast<std::streamsize>(sizeof(r_index_t)) * gfxWorld->draw.indexCount);
            }
        }
    }
} // namespace gfx_world
