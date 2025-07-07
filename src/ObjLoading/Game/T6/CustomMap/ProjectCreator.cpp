#include "ProjectCreator.h"

class CustomMapLoader
{
    void parseMesh(objl::Mesh* OBJMesh, worldSurface* meshSurface, customMapInfo* projInfo)
    {
        int meshVertexCount = OBJMesh->Vertices.size();
        int meshIndexCount = OBJMesh->Indices.size();
        int surfVertexStart = projInfo->gfxInfo.vertexCount;
        int surfIndexStart = projInfo->gfxInfo.indexCount;

        projInfo->gfxInfo.vertexCount += meshVertexCount;
        projInfo->gfxInfo.indexCount += meshIndexCount;

        projInfo->gfxInfo.vertices = (customMapVertex*)realloc(projInfo->gfxInfo.vertices, sizeof(customMapVertex) * projInfo->gfxInfo.vertexCount);
        projInfo->gfxInfo.indices = (uint16_t*)realloc(projInfo->gfxInfo.indices, sizeof(uint16_t) * projInfo->gfxInfo.indexCount);

        customMapVertex* surfVertices = &projInfo->gfxInfo.vertices[surfVertexStart];
        uint16_t* surfIndices = &projInfo->gfxInfo.indices[surfIndexStart];

        meshSurface->firstIndex_Index = surfIndexStart;
        meshSurface->firstVertexIndex = surfVertexStart;
        meshSurface->triCount = meshIndexCount / 3;
        _ASSERT(meshIndexCount % 3 == 0);

        for (int i = 0; i < meshIndexCount; i++)
        {
            _ASSERT(OBJMesh->Indices[i] < UINT16_MAX);
            surfIndices[i] = OBJMesh->Indices[i];
        }

        for (int i = 0; i < meshVertexCount; i++)
        {
            objl::Vertex* meshVertex = &OBJMesh->Vertices[i];
            customMapVertex* surfVertex = &surfVertices[i];

            surfVertex->pos.x = meshVertex->Position.X;
            surfVertex->pos.y = meshVertex->Position.Y;
            surfVertex->pos.z = meshVertex->Position.Z;

            surfVertex->color[0] = OBJMesh->MeshMaterial.Kd.X;
            surfVertex->color[1] = OBJMesh->MeshMaterial.Kd.Y;
            surfVertex->color[2] = OBJMesh->MeshMaterial.Kd.Z;
            surfVertex->color[3] = 1.0f;

            surfVertex->texCoord[0] = meshVertex->TextureCoordinate.X;
            surfVertex->texCoord[1] = meshVertex->TextureCoordinate.Y;

            surfVertex->normal.x = meshVertex->Normal.X;
            surfVertex->normal.y = meshVertex->Normal.Y;
            surfVertex->normal.z = meshVertex->Normal.Z;

            // TODO: fix tangents, seems to work for now though
            surfVertex->tangent.x = 1.0f;
            surfVertex->tangent.y = 0.0f;
            surfVertex->tangent.z = 0.0f;

            surfVertex->packedLmapCoord = 0;

            surfVertex->binormalSign = 0.0f;
        }
    }

    customMapInfo* createCustomMapInfo(std::string& projectName, ISearchPath& searchPath)
    {
        std::string objFilePath = searchPath.GetPath() + "custom_map/world.obj";

        objl::Loader OBJloader;
        bool isLoaded = OBJloader.LoadFile(objFilePath);
        if (!isLoaded)
        {
            printf("OBJLoader: unable to load obj file %s\n", objFilePath.c_str());
            return NULL;
        }

        customMapInfo* projInfo = new customMapInfo;

        projInfo->name = projectName;
        projInfo->bspName = "maps/mp/" + projectName + ".d3dbsp";

        projInfo->gfxInfo.surfaceCount = OBJloader.LoadedMeshes.size();
        projInfo->gfxInfo.surfaces = new worldSurface[projInfo->gfxInfo.surfaceCount];

        projInfo->gfxInfo.vertexCount = 0;
        projInfo->gfxInfo.indexCount = 0;
        projInfo->gfxInfo.vertices = (customMapVertex*)malloc(1);
        projInfo->gfxInfo.indices = (uint16_t*)malloc(1);

        for (int i = 0; i < projInfo->gfxInfo.surfaceCount; i++)
        {
            objl::Mesh* currMesh = &OBJloader.LoadedMeshes[i];
            worldSurface* currSurface = &projInfo->gfxInfo.surfaces[i];

            currSurface->materialName = currMesh->MeshMaterial.name;
            currSurface->reflectionProbeIndex = 0;
            currSurface->primaryLightIndex = 0;
            currSurface->lightmapIndex = 0;
            currSurface->flags = 0;

            parseMesh(currMesh, currSurface, projInfo);
        }

        return projInfo;
    }
};
