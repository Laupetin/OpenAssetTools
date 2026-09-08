#include "ClipMapDumperIW5.h"

#include "Json/JsonCommon.h"
#include "Maps/ClipMapCommon.h"

#include <cstdint>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <string>

using namespace nlohmann;
using namespace IW5;

namespace
{
    json JsonBounds(const Bounds& bounds)
    {
        return json{
            {"midPoint", JsonVec3{bounds.midPoint.v[0], bounds.midPoint.v[1], bounds.midPoint.v[2]}},
            {"halfSize", JsonVec3{bounds.halfSize.v[0], bounds.halfSize.v[1], bounds.halfSize.v[2]}},
        };
    }

    // Indexed by cbrushside_t::materialNum and a brush's axialMaterialNum.
    //
    // surfaceFlags/contents stay raw: no published IW5 header defines those bits, so
    // decoding them here would be guesswork baked into the output.
    void CreateJsonMaterials(json& jRoot, const ClipInfo& info)
    {
        auto jMaterials = json::array();
        if (info.materials)
        {
            for (auto i = 0u; i < info.numMaterials; i++)
            {
                const auto& material = info.materials[i];
                jMaterials.emplace_back(json{
                    {"name",         material.name ? material.name : ""},
                    {"surfaceFlags", material.surfaceFlags             },
                    {"contents",     material.contents                 },
                });
            }
        }
        jRoot["materials"] = std::move(jMaterials);
    }

    // A brush is the intersection of the half-spaces of its sides.
    void CreateJsonBrushes(json& jRoot, const ClipInfo& info)
    {
        auto jBrushes = json::array();
        if (info.brushes)
        {
            for (auto i = 0u; i < static_cast<unsigned>(info.numBrushes); i++)
            {
                const auto& brush = info.brushes[i];

                json jBrush{
                    {"numsides", brush.numsides},
                };

                if (info.brushContents)
                    jBrush["contents"] = info.brushContents[i];
                if (info.brushBounds)
                    jBrush["bounds"] = JsonBounds(info.brushBounds[i]);

                auto jSides = json::array();
                if (brush.sides)
                {
                    for (auto s = 0u; s < brush.numsides; s++)
                    {
                        const auto& side = brush.sides[s];
                        json jSide{
                            {"materialNum", side.materialNum},
                        };
                        if (side.plane)
                        {
                            jSide["normal"] = {side.plane->normal[0], side.plane->normal[1], side.plane->normal[2]};
                            jSide["dist"] = side.plane->dist;
                        }
                        jSides.emplace_back(std::move(jSide));
                    }
                }
                jBrush["sides"] = std::move(jSides);

                jBrushes.emplace_back(std::move(jBrush));
            }
        }
        jRoot["brushes"] = std::move(jBrushes);
    }

    // invScaledAxis is the inverse of the placement's scaled rotation, so a reader
    // recovers the forward transform by inverting it.
    void CreateJsonStaticModels(json& jRoot, const clipMap_t& clipMap)
    {
        auto jModels = json::array();
        if (clipMap.staticModelList)
        {
            for (auto i = 0u; i < clipMap.numStaticModels; i++)
            {
                const auto& model = clipMap.staticModelList[i];

                json jModel{
                    {"origin",    JsonVec3{model.origin[0], model.origin[1], model.origin[2]}},
                    {"absBounds", JsonBounds(model.absBounds)                                },
                };
                if (model.xmodel && model.xmodel->name)
                    jModel["model"] = model.xmodel->name;

                auto jAxis = json::array();
                for (const auto& row : model.invScaledAxis)
                    jAxis.emplace_back(json{row[0], row[1], row[2]});
                jModel["invScaledAxis"] = std::move(jAxis);

                jModels.emplace_back(std::move(jModel));
            }
        }
        jRoot["staticModels"] = std::move(jModels);
    }

    // Destructibles are not in staticModelList: they live in their own two lists with
    // their own contents.
    void CreateJsonDynEnts(json& jRoot, const clipMap_t& clipMap)
    {
        auto jAll = json::array();
        for (auto list = 0u; list < 2u; list++)
        {
            auto jList = json::array();
            const auto* defs = clipMap.dynEntDefList[list];
            if (defs)
            {
                for (auto i = 0u; i < clipMap.dynEntCount[list]; i++)
                {
                    const auto& def = defs[i];
                    json jDef{
                        {"type",     static_cast<int>(def.type)                                             },
                        {"contents", def.contents                                                           },
                        {"health",   def.health                                                             },
                        {"origin",   JsonVec3{def.pose.origin[0], def.pose.origin[1], def.pose.origin[2]}   },
                        {"quat",
                         JsonVec4{def.pose.quat[0], def.pose.quat[1], def.pose.quat[2], def.pose.quat[3]}   },
                    };
                    if (def.xModel && def.xModel->name)
                        jDef["model"] = def.xModel->name;
                    jList.emplace_back(std::move(jDef));
                }
            }
            jAll.emplace_back(std::move(jList));
        }
        jRoot["dynEnts"] = std::move(jAll);
    }

    void WriteManifest(std::ostream& stream, const clipMap_t& clipMap, const std::string& assetName)
    {
        json jRoot{
            {"_game",           "iw5"                                          },
            {"_type",           "clipmap"                                      },
            {"_version",        1                                              },
            {"name",            assetName                                      },
            {"planeCount",      clipMap.info.planeCount                        },
            {"numBrushSides",   clipMap.info.numBrushSides                     },
            {"numLeafBrushes",  clipMap.info.numLeafBrushes                    },
            {"numStaticModels", clipMap.numStaticModels                        },
            {"dynEntCount",     {clipMap.dynEntCount[0], clipMap.dynEntCount[1]}},
            {"numSubModels",    clipMap.numSubModels                           },
            {"vertCount",       clipMap.vertCount                              },
            {"triCount",        clipMap.triCount                               },
            {"partitionCount",  clipMap.partitionCount                         },
            {"borderCount",     clipMap.borderCount                            },
            {"checksum",        clipMap.checksum                               },
        };

        if (clipMap.verts && clipMap.vertCount > 0)
        {
            jRoot["vertFile"] = clip_map::GetVertexFileNameForAssetName(assetName);
            jRoot["vertStride"] = static_cast<unsigned>(sizeof(vec3_t));
        }
        if (clipMap.triIndices && clipMap.triCount > 0)
        {
            jRoot["triFile"] = clip_map::GetTriangleFileNameForAssetName(assetName);
            jRoot["triStride"] = static_cast<unsigned>(sizeof(uint16_t));
        }
        if (clipMap.triEdgeIsWalkable && clipMap.triCount > 0)
            jRoot["edgeWalkableFile"] = clip_map::GetEdgeWalkableFileNameForAssetName(assetName);

        CreateJsonMaterials(jRoot, clipMap.info);
        CreateJsonBrushes(jRoot, clipMap.info);
        CreateJsonStaticModels(jRoot, clipMap);
        CreateJsonDynEnts(jRoot, clipMap);

        stream << std::setw(4) << jRoot << "\n";
    }
} // namespace

namespace clip_map
{
    void DumperIW5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetClipMap::Type>& asset)
    {
        const auto* clipMap = asset.Asset();

        const auto manifestFile = context.OpenAssetFile(GetJsonFileNameForAssetName(asset.m_name));
        if (!manifestFile)
            return;

        WriteManifest(*manifestFile, *clipMap, asset.m_name);

        if (clipMap->verts && clipMap->vertCount > 0)
        {
            if (const auto vertFile = context.OpenAssetFile(GetVertexFileNameForAssetName(asset.m_name)))
            {
                vertFile->write(reinterpret_cast<const char*>(clipMap->verts),
                                static_cast<std::streamsize>(sizeof(vec3_t)) * clipMap->vertCount);
            }
        }

        // triIndices is 3 unsigned shorts per triangle.
        if (clipMap->triIndices && clipMap->triCount > 0)
        {
            if (const auto triFile = context.OpenAssetFile(GetTriangleFileNameForAssetName(asset.m_name)))
            {
                triFile->write(reinterpret_cast<const char*>(clipMap->triIndices),
                               static_cast<std::streamsize>(sizeof(uint16_t)) * 3 * clipMap->triCount);
            }
        }

        // One byte per triangle, bit i marking edge i as walkable.
        if (clipMap->triEdgeIsWalkable && clipMap->triCount > 0)
        {
            if (const auto walkableFile = context.OpenAssetFile(GetEdgeWalkableFileNameForAssetName(asset.m_name)))
            {
                walkableFile->write(reinterpret_cast<const char*>(clipMap->triEdgeIsWalkable),
                                    static_cast<std::streamsize>(clipMap->triCount));
            }
        }
    }
} // namespace clip_map
