#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

#include "Util.h" 
#include "CustomMapConsts.h" 

using TriangleGroup = std::vector<unsigned int>; // Group of triangle indices
using AdjacencyList = std::vector<TriangleGroup>; // Triangle-to-triangle connections

class TriangleSort
{
private:
    static void computeTriCentroid(vec3_t& result, std::vector<vec3_t>& vertices, uint16_t i0, uint16_t i1, uint16_t i2)
    {
        result.x = 0.0f;
        result.y = 0.0f;
        result.z = 0.0f;

        result.x += vertices[i0].x;
        result.y += vertices[i0].y;
        result.z += vertices[i0].z;
        result.x += vertices[i1].x;
        result.y += vertices[i1].y;
        result.z += vertices[i1].z;
        result.x += vertices[i2].x;
        result.y += vertices[i2].y;
        result.z += vertices[i2].z;

        result.x /= 3.0f;
        result.y /= 3.0f;
        result.z /= 3.0f;
    }

    static AdjacencyList buildAdjacencyList(const std::vector<uint16_t>& indices, std::vector<vec3_t>& vertices, int maxTrianglesPerGroup)
    {
        size_t numTriangles = indices.size() / 3;
        AdjacencyList adjList(numTriangles);

        std::vector<vec3_t> centroids;
        for (size_t i = 0; i < numTriangles; i++)
        {
            vec3_t center;
            computeTriCentroid(center, vertices, indices[i * 3], indices[i * 3 + 1], indices[i * 3 + 2]);
            centroids.push_back(center);
        }

        std::vector<bool> visited(numTriangles, false);
        for (size_t i = 0; i < numTriangles; ++i)
        {
            _ASSERT(i < MAX_COL_VERTS);

            if (visited[i] == true)
                continue;
            visited[i] = true;

            // Store distances to other triangles
            std::vector<std::pair<float, unsigned int>> distances;
            for (size_t j = 0; j < numTriangles; ++j)
            {
                if (visited[j] == true)
                    continue;

                if (i != j)
                {
                    float dist = CMUtil::distBetweenPoints(centroids[i], centroids[j]);
                    distances.emplace_back(dist, j);
                }
            }

            // Sort by distance and take the closest maxNeighbors
            std::sort(distances.begin(), distances.end());
            for (size_t k = 0; k < std::min((size_t)maxTrianglesPerGroup, distances.size()); ++k)
            {
                unsigned int neighbour = distances[k].second;
                adjList[i].push_back(neighbour);
                visited[neighbour] = true;
            }
        }

        for (size_t i = 0; i < visited.size(); i++)
        {
            if (visited[i] == false)
                printf("WARN: missing triangle: %i\n", i);
        }

        return adjList;
    }

public:
    static AdjacencyList groupTriangles(std::vector<uint16_t>& indices, std::vector<vec3_t>& vertices, int maxTrianglesPerGroup)
    {
        return buildAdjacencyList(indices, vertices, maxTrianglesPerGroup);
    }
};