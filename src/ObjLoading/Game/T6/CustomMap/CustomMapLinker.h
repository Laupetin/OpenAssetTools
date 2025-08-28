#pragma once

#include "BinarySpacePartitionTreePreCalc.h"
#include "TriangleSort.h"
#include "CustomMapConsts.h"
#include "Util.h"
#include "Utils/Pack.h"
#include <float.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class CustomMapLinker
{
public:
    CustomMapLinker(MemoryManager& memory, ISearchPath& searchPath, Zone& zone, AssetCreationContext& context)
        : m_memory(memory),
          m_search_path(searchPath),
          m_zone(zone),
          m_context(context)
    {
        hasLinkFailed = false;
    }

    bool linkCustomMap(customMapInfo* projInfo)
    {
        _ASSERT(projInfo != NULL);

        createGfxWorld(projInfo);
        createComWorld(projInfo);
        createMapEnts(projInfo);
        createGameWorldMp(projInfo);
        createSkinnedVerts(projInfo);
        createClipMap(projInfo); // must go last (requires gfx and mapents asset)
        
        checkAndAddDefaultRequiredAssets(projInfo);
        
        if (hasLinkFailed)
        {
            printf("Custom Map link has failed.\n");
            return false;
        }

        return true;
    }

private:
    MemoryManager& m_memory;
    ISearchPath& m_search_path;
    Zone& m_zone;
    AssetCreationContext& m_context;

    bool hasLinkFailed;

    // TODO vd1:
    //	used for UVs of sub-textures, when it is set to empty all of them turn a blank colour
    //	could fix by removing sub textures or figure out how they are created and redo that
    //	its not an important issue though
    bool overwriteDrawData(customMapInfo* projInfo, GfxWorld* gfxWorld)
    {
        int vertexCount = projInfo->gfxInfo.vertexCount;
        customMapVertex* worldVertices = projInfo->gfxInfo.vertices;

        gfxWorld->draw.vertexCount = vertexCount;
        gfxWorld->draw.vertexDataSize0 = vertexCount * sizeof(GfxPackedWorldVertex);
        GfxPackedWorldVertex* vertexBuffer = new GfxPackedWorldVertex[vertexCount];
        for (int i = 0; i < vertexCount; i++)
        {
            customMapVertex* WorldVertex = &worldVertices[i];
            GfxPackedWorldVertex* GfxVertex = &vertexBuffer[i];

            GfxVertex->xyz = CMUtil::convertToBO2Coords(WorldVertex->pos);
            //GfxVertex->xyz = WorldVertex->pos;

            GfxVertex->binormalSign = WorldVertex->binormalSign;

            GfxVertex->color.packed = pack32::Vec4PackGfxColor(WorldVertex->color);

            GfxVertex->texCoord.packed = pack32::Vec2PackTexCoordsUV(WorldVertex->texCoord);

            GfxVertex->normal.packed = pack32::Vec3PackUnitVecThirdBased(CMUtil::convertToBO2Coords(WorldVertex->normal).v);
            //GfxVertex->normal.packed = pack32::Vec3PackUnitVecThirdBased(WorldVertex->normal.v);

            GfxVertex->tangent.packed = pack32::Vec3PackUnitVecThirdBased(CMUtil::convertToBO2Coords(WorldVertex->tangent).v);
            //GfxVertex->tangent.packed = pack32::Vec3PackUnitVecThirdBased(WorldVertex->tangent.v);

            GfxVertex->lmapCoord.packed = WorldVertex->packedLmapCoord;
        }
        gfxWorld->draw.vd0.data = (char*)vertexBuffer;

        // we don't use vd1 but still needs to be initialised
        // the data type varies and 0x20 is enough for all types
        gfxWorld->draw.vertexDataSize1 = 0x20;
        gfxWorld->draw.vd1.data = new char[gfxWorld->draw.vertexDataSize1];
        memset(gfxWorld->draw.vd1.data, 0, gfxWorld->draw.vertexDataSize1);

        int indexCount = projInfo->gfxInfo.indexCount;
        _ASSERT(indexCount % 3 == 0);
        gfxWorld->draw.indexCount = indexCount;
        gfxWorld->draw.indices = new uint16_t[indexCount];
        for (int i = 0; i < indexCount; i += 3)
        {
            // the editor orders their vertices opposite to bo2, so its converted here
            gfxWorld->draw.indices[i + 2] = projInfo->gfxInfo.indices[i + 0];
            gfxWorld->draw.indices[i + 1] = projInfo->gfxInfo.indices[i + 1];
            gfxWorld->draw.indices[i + 0] = projInfo->gfxInfo.indices[i + 2];
        }

        return true;
    }

    struct s_sortedSurf
    {
        int surfaceIndex;
        int vertexCount;
    };

    bool compareSurfaces(s_sortedSurf& surf0, s_sortedSurf& surf1)
    {
        return surf0.vertexCount > surf1.vertexCount;
    }

    void overwriteMapSurfaces(customMapInfo* projInfo, GfxWorld* gfxWorld)
    {
        bool overwriteResult = overwriteDrawData(projInfo, gfxWorld);
        if (!overwriteResult)
            return;

        unsigned int surfaceCount = projInfo->gfxInfo.surfaceCount;
        gfxWorld->surfaceCount = surfaceCount;
        gfxWorld->dpvs.staticSurfaceCount = surfaceCount;
        gfxWorld->dpvs.surfaces = new GfxSurface[surfaceCount];
        for (unsigned int i = 0; i < surfaceCount; i++)
        {
            auto currSurface = &gfxWorld->dpvs.surfaces[i];
            auto objSurface = &projInfo->gfxInfo.surfaces[i];

            currSurface->primaryLightIndex = DEFAULT_SURFACE_LIGHT;
            currSurface->lightmapIndex = DEFAULT_SURFACE_LIGHTMAP;
            currSurface->reflectionProbeIndex = DEFAULT_SURFACE_REFLECTION_PROBE;
            currSurface->flags = objSurface->flags;

            currSurface->tris.triCount = objSurface->triCount;
            currSurface->tris.baseIndex = objSurface->firstIndex_Index;

            currSurface->tris.vertexDataOffset0 = objSurface->firstVertexIndex * sizeof(GfxPackedWorldVertex);
            currSurface->tris.vertexDataOffset1 = 0;

            std::string surfMaterialName;
            switch (objSurface->material.materialType)
            {
            case CM_MATERIAL_TEXTURE:
                surfMaterialName = objSurface->material.materialName;
                break;

            case CM_MATERIAL_COLOUR:
            case NO_COLOUR_OR_TEXTURE:
                surfMaterialName = colourOnlyMaterialName;
                break;

            default:
                _ASSERT(false);
            }
            auto* assetInfo = m_context.LoadDependency<AssetMaterial>(surfMaterialName);
            if (assetInfo == NULL)
            {
                assetInfo = m_context.LoadDependency<AssetMaterial>(missingMaterialName);
                if (assetInfo == NULL)
                {
                    printf("Error: unable to find the default texture!\n"); 
                    hasLinkFailed = true;
                    return;
                }
            }
            currSurface->material = assetInfo->Asset();
            
            GfxPackedWorldVertex* firstVert = (GfxPackedWorldVertex*)&gfxWorld->draw.vd0.data[currSurface->tris.vertexDataOffset0];
            currSurface->bounds[0].x = firstVert[0].xyz.x;
            currSurface->bounds[0].y = firstVert[0].xyz.y;
            currSurface->bounds[0].z = firstVert[0].xyz.z;
            currSurface->bounds[1].x = firstVert[0].xyz.x;
            currSurface->bounds[1].y = firstVert[0].xyz.y;
            currSurface->bounds[1].z = firstVert[0].xyz.z;
            for (int k = 0; k < currSurface->tris.triCount * 3; k++)
            {
                uint16_t vertIndex = gfxWorld->draw.indices[currSurface->tris.baseIndex + k];
                CMUtil::calcNewBoundsWithPoint(&firstVert[vertIndex].xyz, &currSurface->bounds[0], &currSurface->bounds[1]);
            }

            // unused values
            currSurface->tris.mins.x = 0.0f;
            currSurface->tris.mins.y = 0.0f;
            currSurface->tris.mins.z = 0.0f;
            currSurface->tris.maxs.x = 0.0f;
            currSurface->tris.maxs.y = 0.0f;
            currSurface->tris.maxs.z = 0.0f;
            currSurface->tris.himipRadiusInvSq = 0.0f;
            currSurface->tris.vertexCount = 0;
            currSurface->tris.firstVertex = 0;
        }

        // doesn't seem to matter what order the sorted surfs go in
        gfxWorld->dpvs.sortedSurfIndex = new uint16_t[surfaceCount];
        for (unsigned int i = 0; i < surfaceCount; i++)
        {
            gfxWorld->dpvs.sortedSurfIndex[i] = i;
        }

        gfxWorld->dpvs.surfaceMaterials = new GfxDrawSurf_align4[surfaceCount];
        memset(gfxWorld->dpvs.surfaceMaterials, 0, sizeof(GfxDrawSurf_align4) * surfaceCount);

        // all visdata is alligned by 128
        gfxWorld->dpvs.surfaceVisDataCount = CMUtil::allignBy128(surfaceCount);
        gfxWorld->dpvs.surfaceVisData[0] = new char[surfaceCount];
        gfxWorld->dpvs.surfaceVisData[1] = new char[surfaceCount];
        gfxWorld->dpvs.surfaceVisData[2] = new char[surfaceCount];
        gfxWorld->dpvs.surfaceVisDataCameraSaved = new char[surfaceCount];
        gfxWorld->dpvs.surfaceCastsShadow = new char[surfaceCount];
        gfxWorld->dpvs.surfaceCastsSunShadow = new char[surfaceCount];
        memset(gfxWorld->dpvs.surfaceVisData[0], 0, surfaceCount);
        memset(gfxWorld->dpvs.surfaceVisData[1], 0, surfaceCount);
        memset(gfxWorld->dpvs.surfaceVisData[2], 0, surfaceCount);
        memset(gfxWorld->dpvs.surfaceVisDataCameraSaved, 0, surfaceCount);
        memset(gfxWorld->dpvs.surfaceCastsShadow, 0, surfaceCount);
        memset(gfxWorld->dpvs.surfaceCastsSunShadow, 0, surfaceCount);

        gfxWorld->dpvs.litSurfsBegin = 0;
        gfxWorld->dpvs.litSurfsEnd = surfaceCount;
        gfxWorld->dpvs.emissiveOpaqueSurfsBegin = surfaceCount;
        gfxWorld->dpvs.emissiveOpaqueSurfsEnd = surfaceCount;
        gfxWorld->dpvs.emissiveTransSurfsBegin = surfaceCount;
        gfxWorld->dpvs.emissiveTransSurfsEnd = surfaceCount;
        gfxWorld->dpvs.litTransSurfsBegin = surfaceCount;
        gfxWorld->dpvs.litTransSurfsEnd = surfaceCount;
    }

    void overwriteMapSModels(customMapInfo* projInfo, GfxWorld* gfxWorld)
    {
        unsigned int modelCount = projInfo->modelCount;
        gfxWorld->dpvs.smodelCount = modelCount;
        gfxWorld->dpvs.smodelInsts = new GfxStaticModelInst[modelCount];
        gfxWorld->dpvs.smodelDrawInsts = new GfxStaticModelDrawInst[modelCount];
        
        for (unsigned int i = 0; i < modelCount; i++)
        {
                auto currModel = &gfxWorld->dpvs.smodelDrawInsts[i];
                auto currModelInst = &gfxWorld->dpvs.smodelInsts[i];
                customMapModel* inModel = &projInfo->models[i];

                auto xModelAsset = m_context.LoadDependency<AssetXModel>(inModel->name);
                if (xModelAsset == NULL)
                {
                    printf("XModel %s not found!\n", inModel->name.c_str());
                    currModel->model = NULL;
                }
                else
                    currModel->model = (XModel*)xModelAsset->Asset();

                currModel->placement.origin.x = inModel->origin.x;
                currModel->placement.origin.y = inModel->origin.y;
                currModel->placement.origin.z = inModel->origin.z;
                currModel->placement.origin = CMUtil::convertToBO2Coords(currModel->placement.origin);
                currModel->placement.scale = inModel->scale;

                CMUtil::convertAnglesToAxis(&inModel->rotation, currModel->placement.axis);

                // mins and maxs are calculated in world space not local space
                // TODO: this does not account for model rotation or scale
                currModelInst->mins.x = currModel->model->mins.x + currModel->placement.origin.x;
                currModelInst->mins.y = currModel->model->mins.y + currModel->placement.origin.y;
                currModelInst->mins.z = currModel->model->mins.z + currModel->placement.origin.z;
                currModelInst->maxs.x = currModel->model->maxs.x + currModel->placement.origin.x;
                currModelInst->maxs.y = currModel->model->maxs.y + currModel->placement.origin.y;
                currModelInst->maxs.z = currModel->model->maxs.z + currModel->placement.origin.z;

                currModel->cullDist = DEFAULT_SMODEL_CULL_DIST;
                currModel->flags = DEFAULT_SMODEL_FLAGS;
                currModel->primaryLightIndex = DEFAULT_SMODEL_LIGHT;
                currModel->reflectionProbeIndex = DEFAULT_SMODEL_REFLECTION_PROBE;

                // unknown use / unused
                currModel->smid = i;
                memset(&currModel->lightingSH, 0, sizeof(GfxLightingSHQuantized));
                currModel->invScaleSq = 0.0f;
                currModel->lightingHandle = 0;
                currModel->colorsIndex = 0;
                currModel->visibility = 0;

                // setting these to NULL makes any static/baked lighting go black when not rendered by real-time lighting or in a shadow
                // TODO: calculate lighting and store it here
                currModel->lmapVertexInfo[0].numLmapVertexColors = 0;
                currModel->lmapVertexInfo[0].lmapVertexColors = NULL;
                currModel->lmapVertexInfo[1].numLmapVertexColors = 0;
                currModel->lmapVertexInfo[1].lmapVertexColors = NULL;
                currModel->lmapVertexInfo[2].numLmapVertexColors = 0;
                currModel->lmapVertexInfo[2].lmapVertexColors = NULL;
                currModel->lmapVertexInfo[3].numLmapVertexColors = 0;
                currModel->lmapVertexInfo[3].lmapVertexColors = NULL;
        }
        

        // all visdata is alligned by 128
        gfxWorld->dpvs.smodelVisDataCount = CMUtil::allignBy128(modelCount);
        gfxWorld->dpvs.smodelVisData[0] = new char[modelCount];
        gfxWorld->dpvs.smodelVisData[1] = new char[modelCount];
        gfxWorld->dpvs.smodelVisData[2] = new char[modelCount];
        gfxWorld->dpvs.smodelVisDataCameraSaved = new char[modelCount];
        gfxWorld->dpvs.smodelCastsShadow = new char[modelCount];
        for (unsigned int i = 0; i < modelCount; i++)
        {
            if ((gfxWorld->dpvs.smodelDrawInsts[i].flags & SMODEL_FLAG_NO_SHADOW) == 0)
                gfxWorld->dpvs.smodelCastsShadow[i] = 1;
            else
                gfxWorld->dpvs.smodelCastsShadow[i] = 0;
        }
        memset(gfxWorld->dpvs.smodelVisData[0], 0, modelCount);
        memset(gfxWorld->dpvs.smodelVisData[1], 0, modelCount);
        memset(gfxWorld->dpvs.smodelVisData[2], 0, modelCount);
        memset(gfxWorld->dpvs.smodelVisDataCameraSaved, 0, modelCount);

        gfxWorld->dpvs.usageCount = 0;
    }

    void cleanGfxWorld(GfxWorld* gfxWorld)
    {
        gfxWorld->checksum = 0;

        // Remove Coronas
        gfxWorld->coronaCount = 0;
        gfxWorld->coronas = NULL;

        // Remove exposure volumes
        gfxWorld->exposureVolumeCount = 0;
        gfxWorld->exposureVolumes = NULL;
        gfxWorld->exposureVolumePlaneCount = 0;
        gfxWorld->exposureVolumePlanes = NULL;

        // Remove hero lights
        gfxWorld->heroLightCount = 0;
        gfxWorld->heroLights = NULL;
        gfxWorld->heroLightTreeCount = 0;
        gfxWorld->heroLightTree = NULL;

        // remove LUT data
        gfxWorld->lutVolumeCount = 0;
        gfxWorld->lutVolumes = NULL;
        gfxWorld->lutVolumePlaneCount = 0;
        gfxWorld->lutVolumePlanes = NULL;

        // remove occluders
        gfxWorld->numOccluders = 0;
        gfxWorld->occluders = NULL;

        // remove Siege Skins
        gfxWorld->numSiegeSkinInsts = 0;
        gfxWorld->siegeSkinInsts = NULL;

        // remove outdoor bounds
        gfxWorld->numOutdoorBounds = 0;
        gfxWorld->outdoorBounds = NULL;

        // remove materials
        gfxWorld->ropeMaterial = NULL;
        gfxWorld->lutMaterial = NULL;
        gfxWorld->waterMaterial = NULL;
        gfxWorld->coronaMaterial = NULL;

        // remove shadow maps
        gfxWorld->shadowMapVolumeCount = 0;
        gfxWorld->shadowMapVolumes = NULL;
        gfxWorld->shadowMapVolumePlaneCount = 0;
        gfxWorld->shadowMapVolumePlanes = NULL;

        // remove stream info
        gfxWorld->streamInfo.aabbTreeCount = 0;
        gfxWorld->streamInfo.aabbTrees = NULL;
        gfxWorld->streamInfo.leafRefCount = 0;
        gfxWorld->streamInfo.leafRefs = NULL;

        // remove sun data
        memset(&gfxWorld->sun, 0, sizeof(sunflare_t));
        gfxWorld->sun.hasValidData = false;

        // Remove Water
        gfxWorld->waterDirection = 0.0f;
        gfxWorld->waterBuffers[0].buffer = NULL;
        gfxWorld->waterBuffers[0].bufferSize = NULL;
        gfxWorld->waterBuffers[1].buffer = NULL;
        gfxWorld->waterBuffers[1].bufferSize = NULL;

        // Remove Fog
        gfxWorld->worldFogModifierVolumeCount = 0;
        gfxWorld->worldFogModifierVolumes = NULL;
        gfxWorld->worldFogModifierVolumePlaneCount = 0;
        gfxWorld->worldFogModifierVolumePlanes = NULL;
        gfxWorld->worldFogVolumeCount = 0;
        gfxWorld->worldFogVolumes = NULL;
        gfxWorld->worldFogVolumePlaneCount = 0;
        gfxWorld->worldFogVolumePlanes = NULL;

        // materialMemory is unused
        gfxWorld->materialMemoryCount = 0;
        gfxWorld->materialMemory = NULL;

        // sunLight is overwritten by the game, just needs to be a valid pointer
        gfxWorld->sunLight = new GfxLight;
        memset(gfxWorld->sunLight, 0, sizeof(GfxLight));
    }

    void overwriteGfxLights(GfxWorld* gfxWorld)
    {
        // there must be 2 or more lights, first is the default light and second is the sun
        gfxWorld->primaryLightCount = 2;
        gfxWorld->sunPrimaryLightIndex = SUN_LIGHT_INDEX; // the sun is always index 1

        gfxWorld->shadowGeom = new GfxShadowGeometry[gfxWorld->primaryLightCount];
        for (unsigned int i = 0; i < gfxWorld->primaryLightCount; i++)
        {
            gfxWorld->shadowGeom[i].smodelCount = 0;
            gfxWorld->shadowGeom[i].surfaceCount = 0;
            gfxWorld->shadowGeom[i].smodelIndex = NULL;
            gfxWorld->shadowGeom[i].sortedSurfIndex = NULL;
        }

        gfxWorld->lightRegion = new GfxLightRegion[gfxWorld->primaryLightCount];
        for (unsigned int i = 0; i < gfxWorld->primaryLightCount; i++)
        {
            gfxWorld->lightRegion[i].hullCount = 0;
            gfxWorld->lightRegion[i].hulls = NULL;
        }

        int lightEntShadowVisSize = (gfxWorld->primaryLightCount - gfxWorld->sunPrimaryLightIndex - 1) * 8192;
        if (lightEntShadowVisSize != 0)
        {
            gfxWorld->primaryLightEntityShadowVis = new unsigned int[lightEntShadowVisSize];
            memset(gfxWorld->primaryLightEntityShadowVis, 0, lightEntShadowVisSize * sizeof(unsigned int));
        }
        else
        {
            gfxWorld->primaryLightEntityShadowVis = NULL;
        }
    }

    void overwriteLightGrid(GfxWorld* gfxWorld)
    {
        gfxWorld->lightGrid.mins[0] = 0;
        gfxWorld->lightGrid.mins[1] = 0;
        gfxWorld->lightGrid.mins[2] = 0;
        gfxWorld->lightGrid.maxs[0] = 0;
        gfxWorld->lightGrid.maxs[1] = 0;
        gfxWorld->lightGrid.maxs[2] = 0;

        gfxWorld->lightGrid.rowAxis = 0;              // default value
        gfxWorld->lightGrid.colAxis = 1;              // default value
        gfxWorld->lightGrid.sunPrimaryLightIndex = SUN_LIGHT_INDEX; // the sun is always index 1
        gfxWorld->lightGrid.offset = 0.0f;

        // if rowDataStart's first value is -1, it will not look up any lightmap data
        gfxWorld->lightGrid.rowDataStart = new uint16_t[1];
        *gfxWorld->lightGrid.rowDataStart = (uint16_t)(-1);

        gfxWorld->lightGrid.rawRowDataSize = 0;
        gfxWorld->lightGrid.rawRowData = NULL;

        gfxWorld->lightGrid.colorCount = 0;
        gfxWorld->lightGrid.colors = NULL;

        gfxWorld->lightGrid.coeffCount = 0;
        gfxWorld->lightGrid.coeffs = NULL;

        gfxWorld->lightGrid.entryCount = 0;
        gfxWorld->lightGrid.entries = NULL;

        gfxWorld->lightGrid.skyGridVolumeCount = 0;
        gfxWorld->lightGrid.skyGridVolumes = NULL;
    }

    void updateGfxCells(GfxWorld* gfxWorld)
    {
        // Cells are basically data used to determine what can be seen and what cant be seen
        // Right now custom maps have no optimisation so there is only 1 cell
        int cellCount = 1;

        gfxWorld->cellBitsCount = ((cellCount + 127) >> 3) & 0x1FFFFFF0;

        int cellCasterBitsCount = cellCount * ((cellCount + 31) / 32);
        gfxWorld->cellCasterBits = new unsigned int[cellCasterBitsCount];
        memset(gfxWorld->cellCasterBits, 0x00, cellCasterBitsCount * sizeof(unsigned int));

        gfxWorld->cells = new GfxCell[cellCount];
        gfxWorld->cells[0].portalCount = 0;
        gfxWorld->cells[0].portals = NULL;
        gfxWorld->cells[0].reflectionProbeCount = 0;
        gfxWorld->cells[0].reflectionProbes = NULL;
        gfxWorld->cells[0].mins.x = gfxWorld->mins.x;
        gfxWorld->cells[0].mins.y = gfxWorld->mins.y;
        gfxWorld->cells[0].mins.z = gfxWorld->mins.z;
        gfxWorld->cells[0].maxs.x = gfxWorld->maxs.x;
        gfxWorld->cells[0].maxs.y = gfxWorld->maxs.y;
        gfxWorld->cells[0].maxs.z = gfxWorld->maxs.z;

        // AABB trees are used to detect what should be rendered and what shouldn't
        // Just use the first AABB node to hold all models, no optimisation but all models/surfaces wil lbe drawn
        gfxWorld->cells[0].aabbTreeCount = 1;
        gfxWorld->cells[0].aabbTree = new GfxAabbTree[gfxWorld->cells[0].aabbTreeCount];
        gfxWorld->cells[0].aabbTree[0].childCount = 0;
        gfxWorld->cells[0].aabbTree[0].childrenOffset = 0;
        gfxWorld->cells[0].aabbTree[0].startSurfIndex = 0;
        gfxWorld->cells[0].aabbTree[0].surfaceCount = gfxWorld->surfaceCount;
        gfxWorld->cells[0].aabbTree[0].smodelIndexCount = gfxWorld->dpvs.smodelCount;
        gfxWorld->cells[0].aabbTree[0].smodelIndexes = new unsigned short[gfxWorld->dpvs.smodelCount];
        for (unsigned short i = 0; i < gfxWorld->dpvs.smodelCount; i++)
        {
            gfxWorld->cells[0].aabbTree[0].smodelIndexes[i] = i;
        }
        gfxWorld->cells[0].aabbTree[0].mins.x = gfxWorld->mins.x;
        gfxWorld->cells[0].aabbTree[0].mins.y = gfxWorld->mins.y;
        gfxWorld->cells[0].aabbTree[0].mins.z = gfxWorld->mins.z;
        gfxWorld->cells[0].aabbTree[0].maxs.x = gfxWorld->maxs.x;
        gfxWorld->cells[0].aabbTree[0].maxs.y = gfxWorld->maxs.y;
        gfxWorld->cells[0].aabbTree[0].maxs.z = gfxWorld->maxs.z;

        gfxWorld->dpvsPlanes.cellCount = cellCount;

        int sceneEntCellBitsCount = cellCount * 512;
        gfxWorld->dpvsPlanes.sceneEntCellBits = new unsigned int[sceneEntCellBitsCount];
        memset(gfxWorld->dpvsPlanes.sceneEntCellBits, 0x00, sceneEntCellBitsCount * sizeof(unsigned int));

        // nodes have the struct mnode_t, and there must be at least 1 node
        // Nodes mnode_t.cellIndex indexes gfxWorld->cells
        // and (mnode_t.cellIndex - (world->dpvsPlanes.cellCount + 1) indexes world->dpvsPlanes.planes
        gfxWorld->nodeCount = 1;
        gfxWorld->dpvsPlanes.nodes = new uint16_t[gfxWorld->nodeCount];
        gfxWorld->dpvsPlanes.nodes[0] = 1; // nodes reference cells by index + 1

        // planes are overwritten by the clipmap loading code ingame
        gfxWorld->planeCount = 0;
        gfxWorld->dpvsPlanes.planes = NULL;
    }

    void updateWorldBounds(GfxWorld* gfxWorld)
    {
        gfxWorld->mins.x = 0.0f;
        gfxWorld->mins.y = 0.0f;
        gfxWorld->mins.z = 0.0f;
        gfxWorld->maxs.x = 0.0f;
        gfxWorld->maxs.y = 0.0f;
        gfxWorld->maxs.z = 0.0f;

        for (int i = 0; i < gfxWorld->surfaceCount; i++)
        {
            CMUtil::calcNewBounds(&gfxWorld->dpvs.surfaces[i].bounds[0], &gfxWorld->dpvs.surfaces[i].bounds[1], &gfxWorld->mins, &gfxWorld->maxs);
        }
    }

    void overwriteModels(GfxWorld* gfxWorld)
    {
        // these models are the collision for the entities defined in the mapents asset
        // used for triggers and stuff

        gfxWorld->modelCount = 1;
        gfxWorld->models = new GfxBrushModel[gfxWorld->modelCount];

        // first model is always the world model
        gfxWorld->models[0].startSurfIndex = 0;
        gfxWorld->models[0].surfaceCount = gfxWorld->surfaceCount;
        gfxWorld->models[0].bounds[0].x = gfxWorld->mins.x;
        gfxWorld->models[0].bounds[0].y = gfxWorld->mins.y;
        gfxWorld->models[0].bounds[0].z = gfxWorld->mins.z;
        gfxWorld->models[0].bounds[1].x = gfxWorld->maxs.x;
        gfxWorld->models[0].bounds[1].y = gfxWorld->maxs.y;
        gfxWorld->models[0].bounds[1].z = gfxWorld->maxs.z;
        memset(&gfxWorld->models[0].writable, 0, sizeof(GfxBrushModelWritable));

        // for (int i = 1; i < gfxWorld->modelCount; i++)
        //{
        //	auto currEntModel = &gfxWorld->models[i];
        //	json currEntModelJs = js["entityModels"][i - 1];
        //
        //	currEntModel->bounds[0].x = currEntModelJs["mins"]["x"];
        //	currEntModel->bounds[0].y = currEntModelJs["mins"]["y"];
        //	currEntModel->bounds[0].z = currEntModelJs["mins"]["z"];
        //	currEntModel->bounds[1].x = currEntModelJs["maxs"]["x"];
        //	currEntModel->bounds[1].y = currEntModelJs["maxs"]["y"];
        //	currEntModel->bounds[1].z = currEntModelJs["maxs"]["z"];
        //
        //	currEntModel->surfaceCount = 0;
        //	currEntModel->startSurfIndex = (unsigned int)(-1);
        //	memset(&currEntModel->writable, 0, sizeof(GfxBrushModelWritable));
        // }
    }

    void updateSunData(GfxWorld* gfxWorld)
    {
        // default values taken from mp_dig
        gfxWorld->sunParse.fogTransitionTime = (float)0.001;
        gfxWorld->sunParse.name[0] = 0x00;

        gfxWorld->sunParse.initWorldSun->control = 0;
        gfxWorld->sunParse.initWorldSun->exposure = 2.5f;
        gfxWorld->sunParse.initWorldSun->angles.x = -29.0f;
        gfxWorld->sunParse.initWorldSun->angles.y = 254.0f;
        gfxWorld->sunParse.initWorldSun->angles.z = 0.0f;
        gfxWorld->sunParse.initWorldSun->sunCd.x = 1.0f;
        gfxWorld->sunParse.initWorldSun->sunCd.y = 0.89f;
        gfxWorld->sunParse.initWorldSun->sunCd.z = 0.69f;
        gfxWorld->sunParse.initWorldSun->sunCd.w = 13.5f;
        gfxWorld->sunParse.initWorldSun->ambientColor.x = 0.0f;
        gfxWorld->sunParse.initWorldSun->ambientColor.y = 0.0f;
        gfxWorld->sunParse.initWorldSun->ambientColor.z = 0.0f;
        gfxWorld->sunParse.initWorldSun->ambientColor.w = 0.0f;
        gfxWorld->sunParse.initWorldSun->skyColor.x = 0.0f;
        gfxWorld->sunParse.initWorldSun->skyColor.y = 0.0f;
        gfxWorld->sunParse.initWorldSun->skyColor.z = 0.0f;
        gfxWorld->sunParse.initWorldSun->skyColor.w = 0.0f;
        gfxWorld->sunParse.initWorldSun->sunCs.x = 0.0f;
        gfxWorld->sunParse.initWorldSun->sunCs.y = 0.0f;
        gfxWorld->sunParse.initWorldSun->sunCs.z = 0.0f;
        gfxWorld->sunParse.initWorldSun->sunCs.w = 0.0f;

        gfxWorld->sunParse.initWorldFog->baseDist = 150.0f;
        gfxWorld->sunParse.initWorldFog->baseHeight = -100.0f;
        gfxWorld->sunParse.initWorldFog->fogColor.x = 2.35f;
        gfxWorld->sunParse.initWorldFog->fogColor.y = 3.10f;
        gfxWorld->sunParse.initWorldFog->fogColor.z = 3.84f;
        gfxWorld->sunParse.initWorldFog->fogOpacity = 0.52f;
        gfxWorld->sunParse.initWorldFog->halfDist = 4450.f;
        gfxWorld->sunParse.initWorldFog->halfHeight = 2000.f;
        gfxWorld->sunParse.initWorldFog->sunFogColor.x = 5.27f;
        gfxWorld->sunParse.initWorldFog->sunFogColor.y = 4.73f;
        gfxWorld->sunParse.initWorldFog->sunFogColor.z = 3.88f;
        gfxWorld->sunParse.initWorldFog->sunFogInner = 0.0f;
        gfxWorld->sunParse.initWorldFog->sunFogOpacity = 0.67f;
        gfxWorld->sunParse.initWorldFog->sunFogOuter = 80.84f;
        gfxWorld->sunParse.initWorldFog->sunFogPitch = -29.0f;
        gfxWorld->sunParse.initWorldFog->sunFogYaw = 254.0f;
    }

    void updateReflectionProbeData(GfxWorld* gfxWorld)
    {
        gfxWorld->draw.reflectionProbeCount = 1;

        gfxWorld->draw.reflectionProbeTextures = new GfxTexture[gfxWorld->draw.reflectionProbeCount];
        memset(gfxWorld->draw.reflectionProbeTextures, 0, sizeof(GfxTexture) * gfxWorld->draw.reflectionProbeCount);

        gfxWorld->draw.reflectionProbes = new GfxReflectionProbe[gfxWorld->draw.reflectionProbeCount];

        // default values taken from mp_dig
        gfxWorld->draw.reflectionProbes[0].mipLodBias = -8.0;

        gfxWorld->draw.reflectionProbes[0].origin.x = 0.0f;
        gfxWorld->draw.reflectionProbes[0].origin.y = 0.0f;
        gfxWorld->draw.reflectionProbes[0].origin.z = 0.0f;

        gfxWorld->draw.reflectionProbes[0].lightingSH.V0.x = 0.0f;
        gfxWorld->draw.reflectionProbes[0].lightingSH.V0.y = 0.0f;
        gfxWorld->draw.reflectionProbes[0].lightingSH.V0.z = 0.0f;
        gfxWorld->draw.reflectionProbes[0].lightingSH.V0.w = 0.0f;
        gfxWorld->draw.reflectionProbes[0].lightingSH.V1.x = 0.0f;
        gfxWorld->draw.reflectionProbes[0].lightingSH.V1.y = 0.0f;
        gfxWorld->draw.reflectionProbes[0].lightingSH.V1.z = 0.0f;
        gfxWorld->draw.reflectionProbes[0].lightingSH.V1.w = 0.0f;
        gfxWorld->draw.reflectionProbes[0].lightingSH.V2.x = 0.0f;
        gfxWorld->draw.reflectionProbes[0].lightingSH.V2.y = 0.0f;
        gfxWorld->draw.reflectionProbes[0].lightingSH.V2.z = 0.0f;
        gfxWorld->draw.reflectionProbes[0].lightingSH.V2.w = 0.0f;

        gfxWorld->draw.reflectionProbes[0].probeVolumeCount = 0;
        gfxWorld->draw.reflectionProbes[0].probeVolumes = NULL;

        std::string probeImageName = "*reflection_probe0";
        auto probeImageAsset = m_context.LoadDependency<AssetImage>(probeImageName);
        if (probeImageAsset == NULL)
        {
            printf("ERROR! unable to find image %s!\n", probeImageName.c_str());
            hasLinkFailed = true;
            return;
        }
        gfxWorld->draw.reflectionProbes[0].reflectionImage = probeImageAsset->Asset();
    }

    void overwriteLightmapData(GfxWorld* gfxWorld)
    {
        gfxWorld->draw.lightmapCount = 1;

        gfxWorld->draw.lightmapPrimaryTextures = new GfxTexture[gfxWorld->draw.lightmapCount];
        gfxWorld->draw.lightmapSecondaryTextures = new GfxTexture[gfxWorld->draw.lightmapCount];
        gfxWorld->draw.lightmaps = new GfxLightmapArray[gfxWorld->draw.lightmapCount];

        // always set to 0
        memset(gfxWorld->draw.lightmapPrimaryTextures, 0, sizeof(GfxTexture) * gfxWorld->draw.lightmapCount);
        memset(gfxWorld->draw.lightmapSecondaryTextures, 0, sizeof(GfxTexture) * gfxWorld->draw.lightmapCount);

        std::string secondaryTexture = "*lightmap0_secondary";
        auto secondaryTextureAsset = m_context.LoadDependency<AssetImage>(secondaryTexture);
        if (secondaryTextureAsset == NULL)
        {
            printf("ERROR! unable to find lightmap image %s!\n", secondaryTexture.c_str());
            hasLinkFailed = true;
            return;
        }
        gfxWorld->draw.lightmaps[0].primary = NULL; // always NULL
        gfxWorld->draw.lightmaps[0].secondary = secondaryTextureAsset->Asset();
    }

    void overwriteSkyBox(GfxWorld* gfxWorld)
    {
        const char* skyBoxName = "skybox_mp_dig";
        gfxWorld->skyBoxModel = skyBoxName;

        if (m_context.LoadDependency<AssetXModel>(skyBoxName) == NULL)
        {
            printf("WARN: Unable to find the skybox model %s\n", skyBoxName);
        }

        // default skybox values from mp_dig
        gfxWorld->skyDynIntensity.angle0 = 0.0f;
        gfxWorld->skyDynIntensity.angle1 = 0.0f;
        gfxWorld->skyDynIntensity.factor0 = 1.0f;
        gfxWorld->skyDynIntensity.factor1 = 1.0f;
    }

    void updateDynEntData(GfxWorld* gfxWorld)
    {
        gfxWorld->dpvsDyn.dynEntClientCount[0] = DYN_ENT_COUNT + 256;
        gfxWorld->dpvsDyn.dynEntClientCount[1] = 0;
        gfxWorld->dpvsDyn.dynEntClientWordCount[0] = 1; // needs to be at least 1
        gfxWorld->dpvsDyn.dynEntClientWordCount[1] = 0;
        gfxWorld->dpvsDyn.usageCount = 0;

        int dynEntCellBitsSize = gfxWorld->dpvsDyn.dynEntClientWordCount[0] * gfxWorld->dpvsPlanes.cellCount;
        gfxWorld->dpvsDyn.dynEntCellBits[0] = new unsigned int[dynEntCellBitsSize];
        gfxWorld->dpvsDyn.dynEntCellBits[1] = NULL;
        memset(gfxWorld->dpvsDyn.dynEntCellBits[0], 0, sizeof(unsigned int) * dynEntCellBitsSize);

        int dynEntVisData0Size = gfxWorld->dpvsDyn.dynEntClientWordCount[0] * 32;
        gfxWorld->dpvsDyn.dynEntVisData[0][0] = new char[dynEntVisData0Size];
        gfxWorld->dpvsDyn.dynEntVisData[0][1] = new char[dynEntVisData0Size];
        gfxWorld->dpvsDyn.dynEntVisData[0][2] = new char[dynEntVisData0Size];
        gfxWorld->dpvsDyn.dynEntVisData[1][0] = NULL;
        gfxWorld->dpvsDyn.dynEntVisData[1][1] = NULL;
        gfxWorld->dpvsDyn.dynEntVisData[1][2] = NULL;
        memset(gfxWorld->dpvsDyn.dynEntVisData[0][0], 0, dynEntVisData0Size);
        memset(gfxWorld->dpvsDyn.dynEntVisData[0][1], 0, dynEntVisData0Size);
        memset(gfxWorld->dpvsDyn.dynEntVisData[0][2], 0, dynEntVisData0Size);

        int dynEntShadowVisCount = gfxWorld->dpvsDyn.dynEntClientCount[0] * (gfxWorld->primaryLightCount - gfxWorld->sunPrimaryLightIndex - 1);
        gfxWorld->primaryLightDynEntShadowVis[0] = new unsigned int[dynEntShadowVisCount];
        gfxWorld->primaryLightDynEntShadowVis[1] = NULL;
        memset(gfxWorld->primaryLightDynEntShadowVis[0], 0, sizeof(unsigned int) * dynEntShadowVisCount);

        gfxWorld->sceneDynModel = new GfxSceneDynModel[gfxWorld->dpvsDyn.dynEntClientCount[0]];
        gfxWorld->sceneDynBrush = NULL;
        memset(gfxWorld->sceneDynModel, 0, sizeof(GfxSceneDynModel) * gfxWorld->dpvsDyn.dynEntClientCount[0]);
    }

    void updateOutdoors(GfxWorld* gfxWorld)
    {
        float xRecip = 1.0f / (gfxWorld->maxs.x - gfxWorld->mins.x);
        float xScale = -(xRecip * gfxWorld->mins.x);

        float yRecip = 1.0f / (gfxWorld->maxs.y - gfxWorld->mins.y);
        float yScale = -(yRecip * gfxWorld->mins.y);

        float zRecip = 1.0f / (gfxWorld->maxs.z - gfxWorld->mins.z);
        float zScale = -(zRecip * gfxWorld->mins.z);

        memset(gfxWorld->outdoorLookupMatrix, 0, sizeof(gfxWorld->outdoorLookupMatrix));

        gfxWorld->outdoorLookupMatrix[0].x = xRecip;
        gfxWorld->outdoorLookupMatrix[1].y = yRecip;
        gfxWorld->outdoorLookupMatrix[2].z = zRecip;
        gfxWorld->outdoorLookupMatrix[3].x = xScale;
        gfxWorld->outdoorLookupMatrix[3].y = yScale;
        gfxWorld->outdoorLookupMatrix[3].z = zScale;
        gfxWorld->outdoorLookupMatrix[3].w = 1.0f;

        std::string outdoorImageName = std::string("$outdoor");
        auto outdoorImageAsset = m_context.LoadDependency<AssetImage>(outdoorImageName);
        if (outdoorImageAsset == NULL)
        {
            printf("ERROR! unable to find image $outdoor, this will crash your game!\n");
            hasLinkFailed = true;
            return;
        }
        gfxWorld->outdoorImage = outdoorImageAsset->Asset();
    }

    void createGfxWorld(customMapInfo* projInfo)
    {
        GfxWorld* gfxWorld = new GfxWorld;
        gfxWorld->baseName = _strdup(projInfo->name.c_str());
        gfxWorld->name = _strdup(projInfo->bspName.c_str());

        // Default values taken from mp_dig
        gfxWorld->lightingFlags = 0;
        gfxWorld->lightingQuality = 4096;

        cleanGfxWorld(gfxWorld);

        overwriteMapSurfaces(projInfo, gfxWorld);

        overwriteMapSModels(projInfo, gfxWorld);

        overwriteLightmapData(gfxWorld);

        overwriteSkyBox(gfxWorld);

        updateReflectionProbeData(gfxWorld);

        // world bounds are based on surface mins/maxs
        // Other update functions depend on the bounds being set first
        updateWorldBounds(gfxWorld);

        updateOutdoors(gfxWorld);

        // gfx cells depend on surface/smodel count
        updateGfxCells(gfxWorld);

        overwriteLightGrid(gfxWorld);

        overwriteGfxLights(gfxWorld);

        overwriteModels(gfxWorld);

        updateSunData(gfxWorld);

        updateDynEntData(gfxWorld);

        m_context.AddAsset<AssetGfxWorld>(gfxWorld->name, gfxWorld);
    }

    void addXModelsToCollision(customMapInfo* projInfo, clipMap_t* clipMap)
    {
        auto gfxWorldAsset = m_context.LoadDependency<AssetGfxWorld>(projInfo->bspName);
        _ASSERT(gfxWorldAsset != NULL);
        GfxWorld* gfxWorld = gfxWorldAsset->Asset();

        clipMap->numStaticModels = gfxWorld->dpvs.smodelCount;
        clipMap->staticModelList = new cStaticModel_s[clipMap->numStaticModels];

        for (unsigned int i = 0; i < clipMap->numStaticModels; i++)
        {
            GfxStaticModelDrawInst* gfxModelDrawInst = &gfxWorld->dpvs.smodelDrawInsts[i];
            GfxStaticModelInst* gfxModelInst = &gfxWorld->dpvs.smodelInsts[i];
            cStaticModel_s* currModel = &clipMap->staticModelList[i];

            memset(&currModel->writable, 0, sizeof(cStaticModelWritable));
            currModel->xmodel = gfxModelDrawInst->model;
            currModel->contents = gfxModelDrawInst->model->contents;
            currModel->origin.x = gfxModelDrawInst->placement.origin.x;
            currModel->origin.y = gfxModelDrawInst->placement.origin.y;
            currModel->origin.z = gfxModelDrawInst->placement.origin.z;

            // TODO: this does not account for model rotation or scale
            currModel->absmin.x = gfxModelInst->mins.x;
            currModel->absmin.y = gfxModelInst->mins.y;
            currModel->absmin.z = gfxModelInst->mins.z;
            currModel->absmax.x = gfxModelInst->maxs.x;
            currModel->absmax.y = gfxModelInst->maxs.y;
            currModel->absmax.z = gfxModelInst->maxs.z;

            CMUtil::matrixTranspose3x3(gfxModelDrawInst->placement.axis, currModel->invScaledAxis);
            currModel->invScaledAxis[0].x = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[0].x;
            currModel->invScaledAxis[0].y = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[0].y;
            currModel->invScaledAxis[0].z = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[0].z;
            currModel->invScaledAxis[1].x = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[1].x;
            currModel->invScaledAxis[1].y = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[1].y;
            currModel->invScaledAxis[1].z = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[1].z;
            currModel->invScaledAxis[2].x = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[2].x;
            currModel->invScaledAxis[2].y = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[2].y;
            currModel->invScaledAxis[2].z = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[2].z;
        }
    }

    void aabbCalcOriginAndHalfSize(vec3_t* mins, vec3_t* maxs, vec3_t* out_origin, vec3_t* out_halfSize)
    {
        // Origin is the midpoint: (min + max) / 2
        vec3_t temp;
        temp.x = mins->x + maxs->x;
        temp.y = mins->y + maxs->y;
        temp.z = mins->z + maxs->z;
        out_origin->x = temp.x * 0.5f;
        out_origin->y = temp.y * 0.5f;
        out_origin->z = temp.z * 0.5f;

        // Half-size is half the difference: (max - min) / 2
        temp.x = maxs->x - mins->x;
        temp.y = maxs->y - mins->y;
        temp.z = maxs->z - mins->z;
        out_halfSize->x = temp.x * 0.5f;
        out_halfSize->y = temp.y * 0.5f;
        out_halfSize->z = temp.z * 0.5f;
    }

    bool CM_IsEdgeWalkable(clipMap_t* clipMap, int triIndex, int edgeIndex)
    {

        unsigned __int8 edgeBitMask = 1 << ((triIndex + edgeIndex + 2 * triIndex) & 7);

        return (edgeBitMask & clipMap->triEdgeIsWalkable[(triIndex + edgeIndex + 2 * triIndex) >> 3]) != 0;
    }

    void traverseBSPTreeForCounts(BSPTree* node, int* numPlanes, int* numNodes, int* numLeafs, int* numAABBTrees, int* maxObjsPerLeaf)
    {
        if (node->isLeaf)
        {
            (*numLeafs)++;
            // there won't be an AABB tree when objectList is empty
            if (node->u.leaf->getObjectCount() > 0)
            {
                *numAABBTrees += node->u.leaf->getObjectCount() + 1;

                if (node->u.leaf->getObjectCount() > *maxObjsPerLeaf)
                    *maxObjsPerLeaf = node->u.leaf->getObjectCount();
            }
        }
        else
        {
            (*numPlanes)++;
            (*numNodes)++;
            traverseBSPTreeForCounts(node->u.node->front, numPlanes, numNodes, numLeafs, numAABBTrees, maxObjsPerLeaf);
            traverseBSPTreeForCounts(node->u.node->back, numPlanes, numNodes, numLeafs, numAABBTrees, maxObjsPerLeaf);
        }
    }

    vec3_t normalX = {1.0f, 0.0f, 0.0f};
    vec3_t normalY = {0.0f, 1.0f, 0.0f};
    vec3_t normalZ = {0.0f, 0.0f, 1.0f};

    int currPlaneCount = 0;
    int currNodeCount = 0;
    int currLeafCount = 0;
    int currAABBCount = 0;

    int addAABBTreeFromLeaf(BSPTree* node, clipMap_t* clipMap)
    {
        _ASSERT(node->isLeaf);

        int objectCount = node->u.leaf->getObjectCount();
        int firstAABBIndex = currAABBCount;
        currAABBCount += objectCount + 1;

        // calculate root AABB node mins and maxs
        // cannot convert mins and maxs coord to BO2 directly as this will result in incorrect mins and maxs
        // so we have to recompute every min and max, not hard just tedious
        int firstPartitionIndex = node->u.leaf->getObject(0)->partitionIndex;
        auto firstPartition = &clipMap->partitions[firstPartitionIndex];
        uint16_t* firstTri = clipMap->triIndices[firstPartition->firstTri];
        vec3_t* firstVert = &clipMap->verts[firstTri[0]];
        vec3_t aabbMins;
        vec3_t aabbMaxs;
        aabbMins.x = firstVert->x;
        aabbMins.y = firstVert->y;
        aabbMins.z = firstVert->z;
        aabbMaxs.x = firstVert->x;
        aabbMaxs.y = firstVert->y;
        aabbMaxs.z = firstVert->z;
        for (int i = 0; i < objectCount; i++)
        {
            int currPartitionIndex = node->u.leaf->getObject(i)->partitionIndex;
            auto currPartition = &clipMap->partitions[currPartitionIndex];

            for (int k = 0; k < currPartition->triCount; k++)
            {
                uint16_t* tri = clipMap->triIndices[currPartition->firstTri + k];
                for (int l = 0; l < 3; l++)
                {
                    uint16_t vertIndex = tri[l];
                    vec3_t vertCoord = clipMap->verts[vertIndex];
                    CMUtil::calcNewBoundsWithPoint(&vertCoord, &aabbMins, &aabbMaxs);
                }
            }
        }
        CollisionAabbTree* rootAABB = &clipMap->aabbTrees[firstAABBIndex];
        aabbCalcOriginAndHalfSize(&aabbMins, &aabbMaxs, &rootAABB->origin, &rootAABB->halfSize);
        rootAABB->materialIndex = 0;
        rootAABB->childCount = objectCount;
        rootAABB->u.firstChildIndex = firstAABBIndex + 1;

        // populate child AABB nodes
        for (int i = 0; i < objectCount; i++)
        {
            CollisionAabbTree* currAabbTree = &clipMap->aabbTrees[rootAABB->u.firstChildIndex + i];
            int currPartitionIndex = node->u.leaf->getObject(i)->partitionIndex;

            currAabbTree->materialIndex = 0;
            currAabbTree->childCount = 0;
            currAabbTree->u.partitionIndex = currPartitionIndex;

            // calculate partition origin and half size
            CollisionPartition* aabbPartition = &clipMap->partitions[currPartitionIndex];
            uint16_t firstUind = clipMap->info.uinds[aabbPartition->fuind];
            vec3_t* firstVertex = &clipMap->verts[firstUind];
            vec3_t mins;
            vec3_t maxs;
            mins.x = firstVertex->x;
            mins.y = firstVertex->y;
            mins.z = firstVertex->z;
            maxs.x = firstVertex->x;
            maxs.y = firstVertex->y;
            maxs.z = firstVertex->z;
            for (int i = 1; i < aabbPartition->nuinds; i++)
            {
                uint16_t currUind = clipMap->info.uinds[aabbPartition->fuind + i];
                vec3_t* currVertex = &clipMap->verts[currUind];

                CMUtil::calcNewBoundsWithPoint(currVertex, &mins, &maxs);
            }

            aabbCalcOriginAndHalfSize(&mins, &maxs, &currAabbTree->origin, &currAabbTree->halfSize);
        }

        return firstAABBIndex;
    }

    // returns the index corresponding to the BSPTree* node parsed
    int16_t populateBSPTree_r(clipMap_t* clipMap, BSPTree* node)
    {
        new cplane_s;
        new cNode_t;
        new cLeaf_s;
        new CollisionAabbTree;

        if (node->isLeaf)
        {
            int currLeafIndex = currLeafCount;
            currLeafCount++;
            cLeaf_s* currLeaf = &clipMap->leafs[currLeafIndex];

            currLeaf->cluster = 0;
            currLeaf->brushContents = 0;                       // no brushes used so contents is 0
            currLeaf->terrainContents = LEAF_TERRAIN_CONTENTS; // clipMap->cmodels[0].leaf.terrainContents takes prescedence

            // unused when leafBrushNode == 0
            currLeaf->mins.x = 0.0f;
            currLeaf->mins.y = 0.0f;
            currLeaf->mins.z = 0.0f;
            currLeaf->maxs.x = 0.0f;
            currLeaf->maxs.y = 0.0f;
            currLeaf->maxs.z = 0.0f;
            currLeaf->leafBrushNode = 0;

            if (node->u.leaf->getObjectCount() > 0)
            {
                currLeaf->firstCollAabbIndex = addAABBTreeFromLeaf(node, clipMap);
                currLeaf->collAabbCount = 1;
            }
            else
            {
                currLeaf->firstCollAabbIndex = 0;
                currLeaf->collAabbCount = 0;
            }

            return -1 - currLeafIndex;
        }
        else
        {
            cplane_s* currPlane = &clipMap->info.planes[currPlaneCount];
            currPlaneCount++;

            if (node->u.node->axis == AXIS_X)
            {
                // X is unchanged when going from OGL x -> BO2 x
                currPlane->normal = normalX;

                // converting OGL -> BO2 X coords doesn't change the x coords at all, so
                // the dist stays the same
                currPlane->dist = (float)node->u.node->distance;
            }
            else
            {
                // converting OGL -> BO2 Z coords negates the z coords and sets it to the y coord.
                // convert the z normal to the y normal, but don't negate it. Negative normals don't do
                // what is expected when the game uses them
                _ASSERT(node->u.node->axis == AXIS_Z);
                currPlane->normal = normalY;

                // converting OGL -> BO2 Z coords negates the z coords and sets it to the y coord.
                // just negate it here as it is just the distance from the orgin along the axis
                currPlane->dist = (float)(-node->u.node->distance);
            }

            bool foundType = false;
            if (currPlane->normal.x == 1.0f)
            {
                _ASSERT(!foundType);
                foundType = true;
                currPlane->type = 0;
            }
            else if (currPlane->normal.y == 1.0f)
            {
                _ASSERT(!foundType);
                foundType = true;
                currPlane->type = 1;
            }
            else if (currPlane->normal.z == 1.0f)
            {
                _ASSERT(!foundType);
                foundType = true;
                currPlane->type = 2;
            }
            else
                _ASSERT(foundType);

            currPlane->signbits = 0;
            if (currPlane->normal.x < 0.0f)
                currPlane->signbits |= 1;
            if (currPlane->normal.y < 0.0f)
                currPlane->signbits |= 2;
            if (currPlane->normal.z < 0.0f)
                currPlane->signbits |= 4;

            currPlane->pad[0] = 0;
            currPlane->pad[1] = 0;

            int currNodeIndex = currNodeCount;
            currNodeCount++;
            cNode_t* currNode = &clipMap->nodes[currNodeIndex];

            currNode->plane = currPlane;
            // Reason for the front and back flip (due to the hacky nature of making the mins and maxs work (see createClipMap)):
            // after converting between OGL and BO2 coords and when and updating the normal from Z -> Y,
            // the normal vector flips and objects behind the plane are now in front, and vise versa
            // so the back node now represents the front, and the front node represents the back.
            // Do the OGL -> Bo2 coord change on paper and it will make sense
            if (currPlane->type == 1)
            {
                currNode->children[1] = populateBSPTree_r(clipMap, node->u.node->front);
                currNode->children[0] = populateBSPTree_r(clipMap, node->u.node->back);
            }
            else
            {
                currNode->children[0] = populateBSPTree_r(clipMap, node->u.node->front);
                currNode->children[1] = populateBSPTree_r(clipMap, node->u.node->back);
            }

            return currNodeIndex;
        }
    }

    void populateBSPTree(clipMap_t* clipMap, BSPTree* tree)
    {
        int numPlanes = 0;
        int numNodes = 0;
        int numLeafs = 0;
        int numAABBTrees = 0;
        int maxObjsPerLeaf = 0;

        traverseBSPTreeForCounts(tree, &numPlanes, &numNodes, &numLeafs, &numAABBTrees, &maxObjsPerLeaf);

        printf("Max Objects per leaf: %i\n", maxObjsPerLeaf);

        clipMap->info.planeCount = numPlanes;
        clipMap->info.planes = new cplane_s[clipMap->info.planeCount];
        clipMap->numNodes = numNodes;
        clipMap->nodes = new cNode_t[clipMap->numNodes];
        // aabb trees: each leaf will have their own AABB tree of the objects within it, and the root aabb node will be the parent of every other aabb node.
        // therefore, each aabb tree will be of size (numObjects + 1) as the tree needs a root aabb node to reference it's children.
        clipMap->aabbTreeCount = numAABBTrees;
        clipMap->aabbTrees = new CollisionAabbTree[clipMap->aabbTreeCount];

        currPlaneCount = 0;
        currNodeCount = 0;
        currAABBCount = 0;

        // first leaf is always empty
        clipMap->numLeafs = numLeafs + 1;
        clipMap->leafs = new cLeaf_s[clipMap->numLeafs];
        memset(&clipMap->leafs[0], 0, sizeof(cLeaf_s));
        currLeafCount = 1;

        populateBSPTree_r(clipMap, tree);

        _ASSERT(clipMap->info.planeCount == currPlaneCount);
        _ASSERT(clipMap->numNodes == currNodeCount);
        _ASSERT(clipMap->numLeafs == currLeafCount);
        _ASSERT(clipMap->aabbTreeCount == currAABBCount);
    }

    void createPartitions(customMapInfo* projInfo, clipMap_t* clipMap)
    {
        int collisionVertexCount = projInfo->colInfo.vertexCount;
        std::vector<vec3_t> collisionVertVec;
        for (int i = 0; i < collisionVertexCount; i++)
        {
            collisionVertVec.push_back(CMUtil::convertToBO2Coords(projInfo->colInfo.vertices[i].pos));
            //collisionVertVec.push_back(projInfo->colInfo.vertices[i].pos);
        }
        clipMap->vertCount = collisionVertexCount;
        clipMap->verts = new vec3_t[collisionVertexCount];
        memcpy(clipMap->verts, &collisionVertVec[0], sizeof(vec3_t) * collisionVertexCount);

        // due to tris using uint16_t as the type for indexing the vert array,
        // any vertex count over the uint16_t max means the vertices above it can't be indexed
        if (collisionVertexCount > MAX_COL_VERTS)
        {
            printf("ERROR: collision vertex count %i exceeds the maximum number: %i!\n", collisionVertexCount, MAX_COL_VERTS);
            hasLinkFailed = true;
            return;
        }
        
        std::vector<uint16_t> triIndexVec;
        for (int i = 0; i < projInfo->colInfo.surfaceCount; i++)
        {
            worldSurface* currSurface = &projInfo->colInfo.surfaces[i];
            int triCount = currSurface->triCount;

            for (int k = 0; k < triCount * 3; k += 3)
            {
                int firstIndex_Index = currSurface->firstIndex_Index;
                int firstVertexIndex = currSurface->firstVertexIndex;

                // gfx index bufer starts at 0 for each new mesh, while the clipmap index buffer indexes the entire
                // clipmap verts buffer, so this code updates the indexes to follow that.
                int triIndex0 = projInfo->colInfo.indices[firstIndex_Index + (k + 0)] + firstVertexIndex;
                int triIndex1 = projInfo->colInfo.indices[firstIndex_Index + (k + 1)] + firstVertexIndex;
                int triIndex2 = projInfo->colInfo.indices[firstIndex_Index + (k + 2)] + firstVertexIndex;

                // triangle index ordering is opposite to blenders, so its converted here
                triIndexVec.push_back(triIndex2);
                triIndexVec.push_back(triIndex1);
                triIndexVec.push_back(triIndex0);
            }
        }
        _ASSERT(triIndexVec.size() % 3 == 0);
        clipMap->triCount = triIndexVec.size() / 3;
        clipMap->triIndices = (uint16_t(*)[3])(new uint16_t[triIndexVec.size()]);
        memcpy(clipMap->triIndices, &triIndexVec[0], sizeof(uint16_t) * triIndexVec.size());

        // partitions are made for each triangle, not one for each surface.
        // one for each surface causes physics bugs, as the entire bounding box is considered solid instead of the surface itself (for some reason).
        // so a partition is made for each triangle which removes the physics bugs but likely makes the game run slower
        std::vector<CollisionPartition> partitionVec;
        for (int i = 0; i < projInfo->colInfo.surfaceCount; i++)
        {
            int triCount = projInfo->colInfo.surfaces[i].triCount;
            int firstTriIndex = projInfo->colInfo.surfaces[i].firstIndex_Index / 3;
            for (int k = 0; k < triCount; k++)
            {
                CollisionPartition newPartition;
                newPartition.nuinds = 0; // initialised later
                newPartition.fuind = 0;  // initialised later
                newPartition.triCount = 1;
                newPartition.firstTri = firstTriIndex;
                firstTriIndex += 1;

                partitionVec.push_back(newPartition);
            }
        }
        clipMap->partitionCount = partitionVec.size();
        clipMap->partitions = new CollisionPartition[clipMap->partitionCount];
        memcpy(clipMap->partitions, &partitionVec[0], sizeof(CollisionPartition) * clipMap->partitionCount);
        
        int totalUindCount = 0;
        std::vector<uint16_t> uindVec;
        for (int i = 0; i < clipMap->partitionCount; i++)
        {
            CollisionPartition* currPartition = &clipMap->partitions[i];
            std::vector<uint16_t> uniqueVertVec;
            for (int k = 0; k < currPartition->triCount; k++)
            {
                uint16_t* tri = clipMap->triIndices[currPartition->firstTri + k];
                for (int l = 0; l < 3; l++)
                {
                    bool isVertexIndexUnique = true;
                    uint16_t vertIndex = tri[l];
        
                    for (size_t m = 0; m < uniqueVertVec.size(); m++)
                    {
                        if (uniqueVertVec[m] == vertIndex)
                        {
                            isVertexIndexUnique = false;
                            break;
                        }
                    }
        
                    if (isVertexIndexUnique)
                        uniqueVertVec.push_back(vertIndex);
                }
            }
        
            currPartition->fuind = totalUindCount;
            currPartition->nuinds = (int)uniqueVertVec.size();
            uindVec.insert(uindVec.end(), uniqueVertVec.begin(), uniqueVertVec.end());
            totalUindCount += currPartition->nuinds;
        }
        clipMap->info.nuinds = totalUindCount;
        clipMap->info.uinds = new uint16_t[totalUindCount];
        memcpy(clipMap->info.uinds, &uindVec[0], sizeof(uint16_t) * totalUindCount);
    }

    void createClipMap(customMapInfo* projInfo)
    {
        clipMap_t* clipMap = new clipMap_t;

        clipMap->name = _strdup(projInfo->bspName.c_str());

        std::string mapEntsName = projInfo->bspName.c_str();
        auto mapEntsAsset = m_context.LoadDependency<AssetMapEnts>(mapEntsName);
        _ASSERT(mapEntsAsset != NULL);
        clipMap->mapEnts = mapEntsAsset->Asset();

        clipMap->pInfo = NULL;

        clipMap->box_model.mins.x = 0.0f;
        clipMap->box_model.mins.y = 0.0f;
        clipMap->box_model.mins.z = 0.0f;
        clipMap->box_model.maxs.x = 0.0f;
        clipMap->box_model.maxs.y = 0.0f;
        clipMap->box_model.maxs.z = 0.0f;
        clipMap->box_model.radius = 0.0f;
        clipMap->box_model.info = NULL;

        // for some reason the maxs are negative, and mins are positive
        // float box_mins = 3.4028235e38;
        // float box_maxs = -3.4028235e38;
        // hack: the floats above can't be converted to 32 bit floats, and the game requires them to be exact
        // so we use the hex representation and set it using int pointers
        unsigned int box_mins = 0x7F7FFFFF; 
        unsigned int box_maxs = 0xFF7FFFFF;
        *((unsigned int*)&clipMap->box_model.leaf.mins.x) = box_mins; 
        *((unsigned int*)&clipMap->box_model.leaf.mins.y) = box_mins;
        *((unsigned int*)&clipMap->box_model.leaf.mins.z) = box_mins;
        *((unsigned int*)&clipMap->box_model.leaf.maxs.x) = box_maxs; 
        *((unsigned int*)&clipMap->box_model.leaf.maxs.y) = box_maxs;
        *((unsigned int*)&clipMap->box_model.leaf.maxs.z) = box_maxs;
        
        clipMap->box_model.leaf.brushContents = -1;
        clipMap->box_model.leaf.terrainContents = 0;
        clipMap->box_model.leaf.cluster = 0;
        clipMap->box_model.leaf.collAabbCount = 0;
        clipMap->box_model.leaf.firstCollAabbIndex = 0;
        clipMap->box_model.leaf.leafBrushNode = 0;

        clipMap->box_brush = new cbrush_t;
        clipMap->box_brush->axial_sflags[0][0] = -1;
        clipMap->box_brush->axial_sflags[0][1] = -1;
        clipMap->box_brush->axial_sflags[0][2] = -1;
        clipMap->box_brush->axial_sflags[1][0] = -1;
        clipMap->box_brush->axial_sflags[1][1] = -1;
        clipMap->box_brush->axial_sflags[1][2] = -1;
        clipMap->box_brush->axial_cflags[0][0] = -1;
        clipMap->box_brush->axial_cflags[0][1] = -1;
        clipMap->box_brush->axial_cflags[0][2] = -1;
        clipMap->box_brush->axial_cflags[1][0] = -1;
        clipMap->box_brush->axial_cflags[1][1] = -1;
        clipMap->box_brush->axial_cflags[1][2] = -1;
        clipMap->box_brush->contents = -1;
        clipMap->box_brush->mins.x = 0.0f;
        clipMap->box_brush->mins.y = 0.0f;
        clipMap->box_brush->mins.z = 0.0f;
        clipMap->box_brush->maxs.x = 0.0f;
        clipMap->box_brush->maxs.y = 0.0f;
        clipMap->box_brush->maxs.z = 0.0f;
        clipMap->box_brush->numsides = 0;
        clipMap->box_brush->numverts = 0;
        clipMap->box_brush->sides = NULL;
        clipMap->box_brush->verts = NULL;

        clipMap->numClusters = 1;
        clipMap->vised = 0;
        clipMap->clusterBytes = ((clipMap->numClusters + 63) >> 3) & 0xFFFFFFF8;
        clipMap->visibility = new char[clipMap->clusterBytes];
        memset(clipMap->visibility, 0xFF, clipMap->clusterBytes);

        clipMap->isInUse = true;
        clipMap->checksum = 0;

        clipMap->num_constraints = 0;
        clipMap->constraints = NULL;
        clipMap->max_ropes = 32;
        clipMap->ropes = new rope_t[clipMap->max_ropes];
        memset(clipMap->ropes, 0, sizeof(rope_t) * clipMap->max_ropes);

        clipMap->info.numBrushSides = 0;
        clipMap->info.brushsides = NULL;
        clipMap->info.leafbrushNodesCount = 0;
        clipMap->info.leafbrushNodes = NULL;
        clipMap->info.numLeafBrushes = 0;
        clipMap->info.leafbrushes = NULL;
        clipMap->info.numBrushVerts = 0;
        clipMap->info.brushVerts = NULL;
        clipMap->info.numBrushes = NULL;
        clipMap->info.brushes = NULL;
        clipMap->info.brushBounds = NULL;
        clipMap->info.brushContents = NULL;

        clipMap->originalDynEntCount = DYN_ENT_COUNT;
        clipMap->dynEntCount[0] = clipMap->originalDynEntCount + 256;
        clipMap->dynEntCount[1] = 0;
        clipMap->dynEntCount[2] = 0;
        clipMap->dynEntCount[3] = 0;

        // assume that there are 0 dyn ents from here on
        clipMap->dynEntClientList[0] = new DynEntityClient[clipMap->dynEntCount[0]];
        clipMap->dynEntClientList[1] = NULL;
        memset(clipMap->dynEntClientList[0], 0, sizeof(DynEntityClient) * clipMap->dynEntCount[0]);

        clipMap->dynEntServerList[0] = NULL;
        clipMap->dynEntServerList[1] = NULL;

        clipMap->dynEntCollList[0] = new DynEntityColl[clipMap->dynEntCount[0]];
        clipMap->dynEntCollList[1] = NULL;
        clipMap->dynEntCollList[2] = NULL;
        clipMap->dynEntCollList[3] = NULL;
        memset(clipMap->dynEntCollList[0], 0, sizeof(DynEntityColl) * clipMap->dynEntCount[0]);

        clipMap->dynEntPoseList[0] = new DynEntityPose[clipMap->dynEntCount[0]];
        clipMap->dynEntPoseList[1] = NULL;
        memset(clipMap->dynEntPoseList[0], 0, sizeof(DynEntityPose) * clipMap->dynEntCount[0]);

        clipMap->dynEntDefList[0] = new DynEntityDef[clipMap->dynEntCount[0]];
        clipMap->dynEntDefList[1] = NULL;
        memset(clipMap->dynEntDefList[0], 0, sizeof(DynEntityDef) * clipMap->dynEntCount[0]);
        for (int i = 0; i < clipMap->dynEntCount[0]; i++)
        {
            DynEntityDef* currDef = &clipMap->dynEntDefList[0][i];
            currDef->physConstraints[0] = 0x1FF;
            currDef->physConstraints[1] = 0x1FF;
            currDef->physConstraints[2] = 0x1FF;
            currDef->physConstraints[3] = 0x1FF;
        }

        // cmodels is the collision for mapents
        auto gfxWorldAsset = m_context.LoadDependency<AssetGfxWorld>(projInfo->bspName);
        _ASSERT(gfxWorldAsset != NULL);
        GfxWorld* gfxWorld = gfxWorldAsset->Asset();
        _ASSERT(gfxWorld->modelCount == 1);
        clipMap->numSubModels = 1;
        clipMap->cmodels = new cmodel_t[clipMap->numSubModels];
        clipMap->cmodels[0].leaf.firstCollAabbIndex = 0;
        clipMap->cmodels[0].leaf.collAabbCount = 0;
        clipMap->cmodels[0].leaf.brushContents = 0;
        clipMap->cmodels[0].leaf.terrainContents = WORLD_TERRAIN_CONTENTS;
        clipMap->cmodels[0].leaf.mins.x = 0.0f;
        clipMap->cmodels[0].leaf.mins.y = 0.0f;
        clipMap->cmodels[0].leaf.mins.z = 0.0f;
        clipMap->cmodels[0].leaf.maxs.x = 0.0f;
        clipMap->cmodels[0].leaf.maxs.y = 0.0f;
        clipMap->cmodels[0].leaf.maxs.z = 0.0f;
        clipMap->cmodels[0].leaf.leafBrushNode = 0;
        clipMap->cmodels[0].leaf.cluster = 0;
        clipMap->cmodels[0].info = NULL;
        clipMap->cmodels[0].mins.x = gfxWorld->models[0].bounds[0].x;
        clipMap->cmodels[0].mins.y = gfxWorld->models[0].bounds[0].y;
        clipMap->cmodels[0].mins.z = gfxWorld->models[0].bounds[0].z;
        clipMap->cmodels[0].maxs.x = gfxWorld->models[0].bounds[1].x;
        clipMap->cmodels[0].maxs.y = gfxWorld->models[0].bounds[1].y;
        clipMap->cmodels[0].maxs.z = gfxWorld->models[0].bounds[1].z;
        clipMap->cmodels[0].radius = CMUtil::distBetweenPoints(clipMap->cmodels[0].mins, clipMap->cmodels[0].maxs) / 2;


        addXModelsToCollision(projInfo, clipMap);

        clipMap->info.numMaterials = 1;
        clipMap->info.materials = new ClipMaterial[clipMap->info.numMaterials];
        clipMap->info.materials[0].name = _strdup(missingMaterialName.c_str());
        clipMap->info.materials[0].contentFlags = MATERIAL_CONTENT_FLAGS;
        clipMap->info.materials[0].surfaceFlags = MATERIAL_SURFACE_FLAGS;

        // set all edges to walkable (all walkable edge bits are set to 1, see isEdgeWalkable) until changing it is a possiblility
        // might do weird stuff on walls, but from testing doesnt seem to do much
        int walkableEdgeSize = (3 * clipMap->triCount + 31) / 32 * 4;
        clipMap->triEdgeIsWalkable = new char[walkableEdgeSize];
        memset(clipMap->triEdgeIsWalkable, 1, walkableEdgeSize * sizeof(char));

        // clipmap BSP creation must go last as it depends on unids, tris and verts already being populated
        // HACK:
        // the BSP tree creation does not work when BO2's coordinate system is used for mins and maxs.
        // Workaround is to convert every BO2 coordinate to OGL's before it is added into the BSP tree,
        // and then convert them back when it is being parsed into the clipmap. Requires some hacky
        // logic, check populateBSPTree_r and addAABBTreeFromLeaf

        createPartitions(projInfo, clipMap);

        vec3_t* firstVert = &clipMap->verts[0];
        vec3_t clipMins;
        vec3_t clipMaxs;
        clipMins.x = firstVert->x;
        clipMins.y = firstVert->y;
        clipMins.z = firstVert->z;
        clipMaxs.x = firstVert->x;
        clipMaxs.y = firstVert->y;
        clipMaxs.z = firstVert->z;
        clipMins = CMUtil::convertFromBO2Coords(clipMins);
        clipMaxs = CMUtil::convertFromBO2Coords(clipMaxs);
        for (unsigned int i = 1; i < clipMap->vertCount; i++)
        {
            vec3_t vertCoord = CMUtil::convertFromBO2Coords(clipMap->verts[i]);
            CMUtil::calcNewBoundsWithPoint(&vertCoord, &clipMins, &clipMaxs);
        }

        BSPTree* tree = new BSPTree(clipMins.x, clipMins.y, clipMins.z, clipMaxs.x, clipMaxs.y, clipMaxs.z, 0);

        _ASSERT(!tree->isLeaf);
        
        for (int i = 0; i < clipMap->partitionCount; i++)
        {
            auto currPartition = &clipMap->partitions[i];

            uint16_t* firstTri = clipMap->triIndices[currPartition->firstTri];
            vec3_t* firstVert = &clipMap->verts[firstTri[0]];
            vec3_t mins;
            vec3_t maxs;
            mins.x = firstVert->x;
            mins.y = firstVert->y;
            mins.z = firstVert->z;
            maxs.x = firstVert->x;
            maxs.y = firstVert->y;
            maxs.z = firstVert->z;
            mins = CMUtil::convertFromBO2Coords(mins);
            maxs = CMUtil::convertFromBO2Coords(maxs);
            for (int k = 0; k < currPartition->triCount; k++)
            {
                uint16_t* tri = clipMap->triIndices[currPartition->firstTri + k];
                for (int l = 0; l < 3; l++)
                {
                    uint16_t vertIndex = tri[l];
                    vec3_t vertCoord = CMUtil::convertFromBO2Coords(clipMap->verts[vertIndex]);
                    CMUtil::calcNewBoundsWithPoint(&vertCoord, &mins, &maxs);
                }
            }

            Object* currObject = new Object(mins.x, mins.y, mins.z, maxs.x, maxs.y, maxs.z, i);

            tree->addObject(currObject);
        }

        populateBSPTree(clipMap, tree);

        m_context.AddAsset<AssetClipMapPvs>(clipMap->name, clipMap);
    }

    void createComWorld(customMapInfo* projInfo)
    {
        // all lights that aren't the sunlight or default light need their own GfxLightDef asset
        ComWorld* comWorld = new ComWorld;
        comWorld->name = _strdup(projInfo->bspName.c_str());
        comWorld->isInUse = 1;
        comWorld->primaryLightCount = 2;
        comWorld->primaryLights = new ComPrimaryLight[comWorld->primaryLightCount];

        // default light is always empty
        ComPrimaryLight* defaultLight = &comWorld->primaryLights[0];
        memset(defaultLight, 0, sizeof(ComPrimaryLight));

        ComPrimaryLight* sunLight = &comWorld->primaryLights[1];
        memset(sunLight, 0, sizeof(ComPrimaryLight));
        sunLight->type = 1;
        sunLight->diffuseColor.r = 0.75f;
        sunLight->diffuseColor.g = 0.75f;
        sunLight->diffuseColor.b = 0.75f;
        sunLight->diffuseColor.a = 1.0f;
        sunLight->dir.x = 0.0f;
        sunLight->dir.y = 0.0f;
        sunLight->dir.z = 0.0f;

        m_context.AddAsset<AssetComWorld>(comWorld->name, comWorld);
    }

    void createMapEnts(customMapInfo* projInfo)
    {
        MapEnts* mapEnts = new MapEnts;

        mapEnts->name = _strdup(projInfo->bspName.c_str());

        // don't need these
        mapEnts->trigger.count = 0;
        mapEnts->trigger.models = NULL;
        mapEnts->trigger.hullCount = 0;
        mapEnts->trigger.hulls = NULL;
        mapEnts->trigger.slabCount = 0;
        mapEnts->trigger.slabs = NULL;

        const auto file = m_search_path.Open("custom_map/entities.json");
        if (!file.IsOpen())
        {
            printf("WARN: can't find entity json!\n");
            return;
        }

        json entJs = json::parse(*file.m_stream);

        std::string entityString;
        int entityCount = entJs["entityCount"];
        for (int i = 0; i < entityCount; i++)
        {
            auto currEntity = entJs["entities"][i];

            if (i == 0)
            {
                std::string className = currEntity["classname"];
                if (className.compare("worldspawn") != 0)
                {
                    printf("ERROR: first entity in the map entity string must be the worldspawn class!");
                    hasLinkFailed = true;
                    return;
                }
            }

            entityString.append("{\n");

            for (auto& element : currEntity.items())
            {
                std::string key = element.key();
                std::string value = element.value();
                entityString.append(std::format("\"{}\" \"{}\"\n", key, value));
            }

            entityString.append("}\n");
        }
        entityString.pop_back(); // remove newline character from the end of the string

        mapEnts->entityString = _strdup(entityString.c_str());
        mapEnts->numEntityChars = entityString.length() + 1; // numEntityChars includes the null character

        m_context.AddAsset<AssetMapEnts>(mapEnts->name, mapEnts);
    }

    void createGameWorldMp(customMapInfo* projInfo)
    {
        GameWorldMp* gameWorldMp = new GameWorldMp;

        gameWorldMp->name = _strdup(projInfo->bspName.c_str());

        gameWorldMp->path.nodeCount = 0;
        gameWorldMp->path.originalNodeCount = 0;
        gameWorldMp->path.visBytes = 0;
        gameWorldMp->path.smoothBytes = 0;
        gameWorldMp->path.nodeTreeCount = 0;

        int nodeCount = gameWorldMp->path.nodeCount + 128;
        gameWorldMp->path.nodes = new pathnode_t[nodeCount];
        gameWorldMp->path.basenodes = new pathbasenode_t[nodeCount];
        memset(gameWorldMp->path.nodes, 0, nodeCount * sizeof(pathnode_t));
        memset(gameWorldMp->path.basenodes, 0, nodeCount * sizeof(pathbasenode_t));

        gameWorldMp->path.pathVis = NULL;
        gameWorldMp->path.smoothCache = NULL;
        gameWorldMp->path.nodeTree = NULL;

        m_context.AddAsset<AssetGameWorldMp>(gameWorldMp->name, gameWorldMp);
    }

    void createSkinnedVerts(customMapInfo* projInfo)
    {
        SkinnedVertsDef* skinnedVerts = new SkinnedVertsDef;
        skinnedVerts->name = "skinnedverts";
        skinnedVerts->maxSkinnedVerts = projInfo->gfxInfo.vertexCount;

        m_context.AddAsset<AssetSkinnedVerts>("skinnedverts", skinnedVerts);
    }

    FootstepTableDef* addEmptyFootstepTableAsset(std::string assetName)
    {
        if (assetName.length() == 0)
            return NULL;

        FootstepTableDef* footstepTable = new FootstepTableDef;
        footstepTable->name = _strdup(assetName.c_str());
        memset(footstepTable->sndAliasTable, 0, sizeof(footstepTable->sndAliasTable));

        m_context.AddAsset<AssetFootstepTable>(assetName, footstepTable);

        return footstepTable;
    }

    void checkAndAddDefaultRequiredAssets(customMapInfo* projectInfo)
    {
        if (m_context.LoadDependency<AssetScript>("maps/mp/mod.gsc") == NULL)
        {
            hasLinkFailed = true;
            return;
        }
        if (m_context.LoadDependency<AssetScript>("maps/mp/mod_amb.gsc") == NULL)
        {
            hasLinkFailed = true;
            return;
        }
        if (m_context.LoadDependency<AssetScript>("maps/mp/mod_fx.gsc") == NULL)
        {
            hasLinkFailed = true;
            return;
        }
        if (m_context.LoadDependency<AssetScript>("maps/mp/createfx/mod_fx.gsc") == NULL)
        {
            hasLinkFailed = true;
            return;
        }
        if (m_context.LoadDependency<AssetScript>("clientscripts/mp/mod.csc") == NULL)
        {
            hasLinkFailed = true;
            return;
        }
        if (m_context.LoadDependency<AssetScript>("clientscripts/mp/mod_amb.csc") == NULL)
        {
            hasLinkFailed = true;
            return;
        }
        if (m_context.LoadDependency<AssetScript>("clientscripts/mp/mod_fx.csc") == NULL)
        {
            hasLinkFailed = true;
            return;
        }
        if (m_context.LoadDependency<AssetScript>("clientscripts/mp/createfx/mod_fx.csc") == NULL)
        {
            hasLinkFailed = true;
            return;
        }

        addEmptyFootstepTableAsset("default_1st_person");
        addEmptyFootstepTableAsset("default_3rd_person");
        addEmptyFootstepTableAsset("default_1st_person_quiet");
        addEmptyFootstepTableAsset("default_3rd_person_quiet");
        addEmptyFootstepTableAsset("default_3rd_person_loud");
        addEmptyFootstepTableAsset("default_ai");
    }
};
