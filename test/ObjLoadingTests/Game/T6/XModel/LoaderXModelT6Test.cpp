#include "Game/T6/XModel/LoaderXModelT6.h"

#include "Game/T6/GameT6.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"
#include "XModel/XModelCommon.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <numeric>
#include <string>

using namespace T6;
using namespace std::literals;

static void ReorderVerticesByWeightCount(std::vector<size_t>& vertexIndices, const XSurface& surface, const XModelCommon& common)
{
    if (common.m_bone_weight_data.weights.empty())
        return;

    const auto vertexCount = vertexIndices.size();
    std::vector<size_t> reorderLookup(vertexCount);
    std::iota(reorderLookup.begin(), reorderLookup.end(), 0);

    std::ranges::sort(reorderLookup,
                      [&common, &vertexIndices](const size_t& i0, const size_t& i1)
                      {
                          const auto& weights0 = common.m_vertex_bone_weights[vertexIndices[i0]];
                          const auto& weights1 = common.m_vertex_bone_weights[vertexIndices[i1]];

                          if (weights0.weightCount < weights1.weightCount)
                              return true;

                          // If there is only one weight, make sure all vertices of the same bone follow another
                          if (weights0.weightCount == 1)
                          {
                              const auto bone0 = common.m_bone_weight_data.weights[weights0.weightOffset].boneIndex;
                              const auto bone1 = common.m_bone_weight_data.weights[weights1.weightOffset].boneIndex;
                              return bone0 < bone1;
                          }

                          return false;
                      });

    std::vector<std::remove_pointer_t<decltype(XSurface::triIndices)>> unsortedTris(surface.triCount);
    std::memcpy(unsortedTris.data(), surface.triIndices, sizeof(std::remove_pointer_t<decltype(XSurface::triIndices)>) * surface.triCount);

    for (auto triIndex = 0u; triIndex < surface.triCount; triIndex++)
    {
        const auto& triIndicesIn = unsortedTris[triIndex];
        auto& triIndicesOut = surface.triIndices[triIndex];

        triIndicesOut.i[0] = static_cast<uint16_t>(reorderLookup[triIndicesIn.i[0]]);
        triIndicesOut.i[1] = static_cast<uint16_t>(reorderLookup[triIndicesIn.i[1]]);
        triIndicesOut.i[2] = static_cast<uint16_t>(reorderLookup[triIndicesIn.i[2]]);
    }

    for (auto& entry : reorderLookup)
        entry = vertexIndices[entry];

    vertexIndices = std::move(reorderLookup);
}

namespace
{
    TEST_CASE("LoaderXModel(T6): Can parse xmodel", "[t6][xmodel][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("xmodel/testmodel.json", R"modeldata(
{
}
)modeldata");

        Zone zone("MockZone", 0, IGame::GetGameById(GameId::T6));

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);
    }

    TEST_CASE("LoaderXModel(T6): Can parse xmodel", "[t6][xmodel][assetloader]")
    {
        std::vector<size_t> data{0, 1, 2};
        XSurface surface{};
        XSurfaceTri16 tris[]{
            {0, 1, 2}
        };
        surface.triIndices = tris;
        surface.triCount = 1;

        XModelCommon common;

        ReorderVerticesByWeightCount(data, surface, common);
    }
} // namespace
