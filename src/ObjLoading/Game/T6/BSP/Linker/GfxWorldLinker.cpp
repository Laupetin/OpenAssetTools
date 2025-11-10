#include "GfxWorldLinker.h"

#include "Game/T6/BSP/BSPUtil.h"
#include "Utils/Alignment.h"
#include "Utils/Pack.h"

#include <cassert>
#include <format>

using namespace T6;

namespace BSP
{
    GfxWorldLinker::GfxWorldLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
          m_search_path(searchPath),
          m_context(context)
    {
    }

    void GfxWorldLinker::loadDrawData(const BSPData& bsp, GfxWorld& gfxWorld) const
    {
        size_t vertexCount = bsp.gfxWorld.vertices.size();
        gfxWorld.draw.vertexCount = static_cast<unsigned int>(vertexCount);
        gfxWorld.draw.vertexDataSize0 = static_cast<unsigned int>(vertexCount * sizeof(GfxPackedWorldVertex));
        GfxPackedWorldVertex* vertexBuffer = m_memory.Alloc<GfxPackedWorldVertex>(vertexCount);
        for (size_t vertIdx = 0; vertIdx < vertexCount; vertIdx++)
        {
            const BSPVertex& bspVertex = bsp.gfxWorld.vertices.at(vertIdx);
            GfxPackedWorldVertex* gfxVertex = &vertexBuffer[vertIdx];

            gfxVertex->xyz = bspVertex.pos;
            gfxVertex->color.packed = pack32::Vec4PackGfxColor(bspVertex.color.v);
            gfxVertex->texCoord.packed = pack32::Vec2PackTexCoordsUV(bspVertex.texCoord.v);
            gfxVertex->normal.packed = pack32::Vec3PackUnitVecThirdBased(bspVertex.normal.v);
            gfxVertex->tangent.packed = pack32::Vec3PackUnitVecThirdBased(bspVertex.tangent.v);

            // unimplemented variables
            gfxVertex->binormalSign = 0.0f;
            gfxVertex->lmapCoord.packed = 0;
        }
        gfxWorld.draw.vd0.data = reinterpret_cast<char*>(vertexBuffer);

        // vd1 is unused but still needs to be initialised
        // the data type varies and 0x20 is enough for all types
        gfxWorld.draw.vertexDataSize1 = 0x20;
        gfxWorld.draw.vd1.data = m_memory.Alloc<char>(gfxWorld.draw.vertexDataSize1);

        size_t indexCount = bsp.gfxWorld.indices.size();
        assert(indexCount % 3 == 0);
        gfxWorld.draw.indexCount = static_cast<int>(indexCount);
        gfxWorld.draw.indices = m_memory.Alloc<uint16_t>(indexCount);
        for (size_t indexIdx = 0; indexIdx < indexCount; indexIdx++)
        {
            gfxWorld.draw.indices[indexIdx] = bsp.gfxWorld.indices.at(indexIdx);
        }
    }

    bool GfxWorldLinker::loadMapSurfaces(const BSPData& bsp, GfxWorld& gfxWorld) const
    {
        loadDrawData(bsp, gfxWorld);

        size_t surfaceCount = bsp.gfxWorld.surfaces.size();
        gfxWorld.surfaceCount = static_cast<int>(surfaceCount);
        gfxWorld.dpvs.staticSurfaceCount = static_cast<unsigned int>(surfaceCount);
        gfxWorld.dpvs.surfaces = m_memory.Alloc<GfxSurface>(surfaceCount);
        for (size_t surfIdx = 0; surfIdx < surfaceCount; surfIdx++)
        {
            const BSPSurface& bspSurface = bsp.gfxWorld.surfaces.at(surfIdx);
            GfxSurface* gfxSurface = &gfxWorld.dpvs.surfaces[surfIdx];

            gfxSurface->primaryLightIndex = BSPEditableConstants::DEFAULT_SURFACE_LIGHT;
            gfxSurface->lightmapIndex = BSPEditableConstants::DEFAULT_SURFACE_LIGHTMAP;
            gfxSurface->reflectionProbeIndex = BSPEditableConstants::DEFAULT_SURFACE_REFLECTION_PROBE;
            gfxSurface->flags = BSPEditableConstants::DEFAULT_SURFACE_FLAGS;

            gfxSurface->tris.triCount = static_cast<uint16_t>(bspSurface.triCount);
            gfxSurface->tris.baseIndex = static_cast<int>(bspSurface.indexOfFirstIndex);

            gfxSurface->tris.vertexDataOffset0 = static_cast<int>(bspSurface.indexOfFirstVertex * sizeof(GfxPackedWorldVertex));
            gfxSurface->tris.vertexDataOffset1 = 0;

            std::string surfMaterialName;
            if (bspSurface.material.materialType == BSPMaterialType::MATERIAL_TYPE_TEXTURE)
                surfMaterialName = bspSurface.material.materialName;
            else // MATERIAL_TYPE_COLOUR || MATERIAL_TYPE_EMPTY
                surfMaterialName = BSPLinkingConstants::COLOR_ONLY_IMAGE_NAME;

            auto surfMaterialAsset = m_context.LoadDependency<AssetMaterial>(surfMaterialName);
            if (!surfMaterialAsset)
            {
                std::string missingImageName = BSPLinkingConstants::MISSING_IMAGE_NAME;
                surfMaterialAsset = m_context.LoadDependency<AssetMaterial>(missingImageName);
                if (!surfMaterialAsset)
                {
                    con::error("unable to load the missing image texture {}!", missingImageName);
                    return false;
                }
            }
            gfxSurface->material = surfMaterialAsset->Asset();

            GfxPackedWorldVertex* firstVert = reinterpret_cast<GfxPackedWorldVertex*>(&gfxWorld.draw.vd0.data[gfxSurface->tris.vertexDataOffset0]);
            gfxSurface->bounds[0].x = firstVert[0].xyz.x;
            gfxSurface->bounds[0].y = firstVert[0].xyz.y;
            gfxSurface->bounds[0].z = firstVert[0].xyz.z;
            gfxSurface->bounds[1].x = firstVert[0].xyz.x;
            gfxSurface->bounds[1].y = firstVert[0].xyz.y;
            gfxSurface->bounds[1].z = firstVert[0].xyz.z;
            for (size_t indexIdx = 0; indexIdx < static_cast<size_t>(gfxSurface->tris.triCount * 3); indexIdx++)
            {
                uint16_t vertIndex = gfxWorld.draw.indices[gfxSurface->tris.baseIndex + indexIdx];
                BSPUtil::updateAABBWithPoint(firstVert[vertIndex].xyz, gfxSurface->bounds[0], gfxSurface->bounds[1]);
            }

            // unused values
            gfxSurface->tris.mins.x = 0.0f;
            gfxSurface->tris.mins.y = 0.0f;
            gfxSurface->tris.mins.z = 0.0f;
            gfxSurface->tris.maxs.x = 0.0f;
            gfxSurface->tris.maxs.y = 0.0f;
            gfxSurface->tris.maxs.z = 0.0f;
            gfxSurface->tris.himipRadiusInvSq = 0.0f;
            gfxSurface->tris.vertexCount = 0;
            gfxSurface->tris.firstVertex = 0;
        }

        // doesn't seem to matter what order the sorted surfs go in
        gfxWorld.dpvs.sortedSurfIndex = m_memory.Alloc<uint16_t>(surfaceCount);
        for (size_t surfIdx = 0; surfIdx < surfaceCount; surfIdx++)
            gfxWorld.dpvs.sortedSurfIndex[surfIdx] = static_cast<uint16_t>(surfIdx);

        // surface materials are written to by the game
        gfxWorld.dpvs.surfaceMaterials = m_memory.Alloc<GfxDrawSurf_align4>(surfaceCount);

        // set all surface types to lit opaque
        gfxWorld.dpvs.litSurfsBegin = 0;
        gfxWorld.dpvs.litSurfsEnd = static_cast<unsigned int>(surfaceCount);
        gfxWorld.dpvs.emissiveOpaqueSurfsBegin = static_cast<unsigned int>(surfaceCount);
        gfxWorld.dpvs.emissiveOpaqueSurfsEnd = static_cast<unsigned int>(surfaceCount);
        gfxWorld.dpvs.emissiveTransSurfsBegin = static_cast<unsigned int>(surfaceCount);
        gfxWorld.dpvs.emissiveTransSurfsEnd = static_cast<unsigned int>(surfaceCount);
        gfxWorld.dpvs.litTransSurfsBegin = static_cast<unsigned int>(surfaceCount);
        gfxWorld.dpvs.litTransSurfsEnd = static_cast<unsigned int>(surfaceCount);

        // visdata is written to by the game
        // all visdata is alligned by 128
        auto alignedSurfaceCount = utils::Align(surfaceCount, 128uz);
        gfxWorld.dpvs.surfaceVisDataCount = static_cast<unsigned int>(alignedSurfaceCount);
        gfxWorld.dpvs.surfaceVisData[0] = m_memory.Alloc<char>(alignedSurfaceCount);
        gfxWorld.dpvs.surfaceVisData[1] = m_memory.Alloc<char>(alignedSurfaceCount);
        gfxWorld.dpvs.surfaceVisData[2] = m_memory.Alloc<char>(alignedSurfaceCount);
        gfxWorld.dpvs.surfaceVisDataCameraSaved = m_memory.Alloc<char>(alignedSurfaceCount);
        gfxWorld.dpvs.surfaceCastsShadow = m_memory.Alloc<char>(alignedSurfaceCount);
        gfxWorld.dpvs.surfaceCastsSunShadow = m_memory.Alloc<char>(alignedSurfaceCount);

        return true;
    }

    void GfxWorldLinker::loadXModels(const BSPData& bsp, GfxWorld& gfxWorld) const
    {
        /*
        Models are unsupported right now
        Code is left in in case it is supported later on

        unsigned int modelCount = projInfo->modelCount;
        gfxWorld.dpvs.smodelCount = modelCount;
        gfxWorld.dpvs.smodelInsts = new GfxStaticModelInst[modelCount];
        gfxWorld.dpvs.smodelDrawInsts = new GfxStaticModelDrawInst[modelCount];

        for (unsigned int i = 0; i < modelCount; i++)
        {
                auto currModel = &gfxWorld.dpvs.smodelDrawInsts[i];
                auto currModelInst = &gfxWorld.dpvs.smodelInsts[i];
                customMapModel* inModel = &projInfo->models[i];

                auto xModelAsset = m_context.LoadDependency<AssetXModel>(inModel->name);
                if (!xModelAsset)
                {
                    printf("XModel %s not found!\n", inModel->name.c_str());
                    currModel->model = nullptr;
                }
                else
                    currModel->model = (XModel*)xModelAsset->Asset();

                currModel->placement.origin.x = inModel->origin.x;
                currModel->placement.origin.y = inModel->origin.y;
                currModel->placement.origin.z = inModel->origin.z;
                currModel->placement.origin = BSPUtil::convertToBO2Coords(currModel->placement.origin);
                currModel->placement.scale = inModel->scale;

                BSPUtil::convertAnglesToAxis(&inModel->rotation, currModel->placement.axis);

                // mins and maxs are calculated in world space not local space
                // TODO: this does not account for model rotation or scale
                currModelInst->mins.x = currModel->model->mins.x + currModel->placement.origin.x;
                currModelInst->mins.y = currModel->model->mins.y + currModel->placement.origin.y;
                currModelInst->mins.z = currModel->model->mins.z + currModel->placement.origin.z;
                currModelInst->maxs.x = currModel->model->maxs.x + currModel->placement.origin.x;
                currModelInst->maxs.y = currModel->model->maxs.y + currModel->placement.origin.y;
                currModelInst->maxs.z = currModel->model->maxs.z + currModel->placement.origin.z;

                currModel->cullDist = DEFAULT_SMODEL_CULL_DIST;
                currModel->flags = BSPEditableConstants::DEFAULT_SMODEL_FLAGS;
                currModel->primaryLightIndex = BSPEditableConstants::DEFAULT_SMODEL_LIGHT;
                currModel->reflectionProbeIndex = BSPEditableConstants::DEFAULT_SMODEL_REFLECTION_PROBE;

                // unknown use / unused
                currModel->smid = i;
                memset(&currModel->lightingSH, 0, sizeof(GfxLightingSHQuantized));
                currModel->invScaleSq = 0.0f;
                currModel->lightingHandle = 0;
                currModel->colorsIndex = 0;
                currModel->visibility = 0;

                // setting these to nullptr makes any static/baked lighting go black when not rendered by real-time lighting or in a shadow
                // TODO: calculate lighting and store it here
                currModel->lmapVertexInfo[0].numLmapVertexColors = 0;
                currModel->lmapVertexInfo[0].lmapVertexColors = nullptr;
                currModel->lmapVertexInfo[1].numLmapVertexColors = 0;
                currModel->lmapVertexInfo[1].lmapVertexColors = nullptr;
                currModel->lmapVertexInfo[2].numLmapVertexColors = 0;
                currModel->lmapVertexInfo[2].lmapVertexColors = nullptr;
                currModel->lmapVertexInfo[3].numLmapVertexColors = 0;
                currModel->lmapVertexInfo[3].lmapVertexColors = nullptr;
        }
        */

        unsigned int modelCount = 0;
        gfxWorld.dpvs.smodelCount = modelCount;
        gfxWorld.dpvs.smodelInsts = m_memory.Alloc<GfxStaticModelInst>(modelCount);
        gfxWorld.dpvs.smodelDrawInsts = m_memory.Alloc<GfxStaticModelDrawInst>(modelCount);

        // visdata is written to by the game
        // all visdata is alligned by 128
        const auto alignedModelCount = utils::Align(modelCount, 128u);
        gfxWorld.dpvs.smodelVisDataCount = static_cast<unsigned int>(alignedModelCount);
        gfxWorld.dpvs.smodelVisData[0] = m_memory.Alloc<char>(alignedModelCount);
        gfxWorld.dpvs.smodelVisData[1] = m_memory.Alloc<char>(alignedModelCount);
        gfxWorld.dpvs.smodelVisData[2] = m_memory.Alloc<char>(alignedModelCount);
        gfxWorld.dpvs.smodelVisDataCameraSaved = m_memory.Alloc<char>(alignedModelCount);
        gfxWorld.dpvs.smodelCastsShadow = m_memory.Alloc<char>(alignedModelCount);
        for (unsigned int i = 0; i < modelCount; i++)
        {
            if ((gfxWorld.dpvs.smodelDrawInsts[i].flags & STATIC_MODEL_FLAG_NO_SHADOW) == 0)
                gfxWorld.dpvs.smodelCastsShadow[i] = 1;
            else
                gfxWorld.dpvs.smodelCastsShadow[i] = 0;
        }

        // official maps set this to 0
        gfxWorld.dpvs.usageCount = 0;
    }

    void GfxWorldLinker::cleanGfxWorld(GfxWorld& gfxWorld) const
    {
        // checksum is generated by the game
        gfxWorld.checksum = 0;

        // Remove Coronas
        gfxWorld.coronaCount = 0;
        gfxWorld.coronas = nullptr;

        // Remove exposure volumes
        gfxWorld.exposureVolumeCount = 0;
        gfxWorld.exposureVolumes = nullptr;
        gfxWorld.exposureVolumePlaneCount = 0;
        gfxWorld.exposureVolumePlanes = nullptr;

        // Remove hero lights
        gfxWorld.heroLightCount = 0;
        gfxWorld.heroLights = nullptr;
        gfxWorld.heroLightTreeCount = 0;
        gfxWorld.heroLightTree = nullptr;

        // remove LUT data
        gfxWorld.lutVolumeCount = 0;
        gfxWorld.lutVolumes = nullptr;
        gfxWorld.lutVolumePlaneCount = 0;
        gfxWorld.lutVolumePlanes = nullptr;

        // remove occluders
        gfxWorld.numOccluders = 0;
        gfxWorld.occluders = nullptr;

        // remove Siege Skins
        gfxWorld.numSiegeSkinInsts = 0;
        gfxWorld.siegeSkinInsts = nullptr;

        // remove outdoor bounds
        gfxWorld.numOutdoorBounds = 0;
        gfxWorld.outdoorBounds = nullptr;

        // remove materials
        gfxWorld.ropeMaterial = nullptr;
        gfxWorld.lutMaterial = nullptr;
        gfxWorld.waterMaterial = nullptr;
        gfxWorld.coronaMaterial = nullptr;

        // remove shadow maps
        gfxWorld.shadowMapVolumeCount = 0;
        gfxWorld.shadowMapVolumes = nullptr;
        gfxWorld.shadowMapVolumePlaneCount = 0;
        gfxWorld.shadowMapVolumePlanes = nullptr;

        // remove stream info
        gfxWorld.streamInfo.aabbTreeCount = 0;
        gfxWorld.streamInfo.aabbTrees = nullptr;
        gfxWorld.streamInfo.leafRefCount = 0;
        gfxWorld.streamInfo.leafRefs = nullptr;

        // remove sun data
        memset(&gfxWorld.sun, 0, sizeof(sunflare_t));
        gfxWorld.sun.hasValidData = false;

        // Remove Water
        gfxWorld.waterDirection = 0.0f;
        gfxWorld.waterBuffers[0].bufferSize = 0;
        gfxWorld.waterBuffers[0].buffer = nullptr;
        gfxWorld.waterBuffers[1].bufferSize = 0;
        gfxWorld.waterBuffers[1].buffer = nullptr;

        // Remove Fog
        gfxWorld.worldFogModifierVolumeCount = 0;
        gfxWorld.worldFogModifierVolumes = nullptr;
        gfxWorld.worldFogModifierVolumePlaneCount = 0;
        gfxWorld.worldFogModifierVolumePlanes = nullptr;
        gfxWorld.worldFogVolumeCount = 0;
        gfxWorld.worldFogVolumes = nullptr;
        gfxWorld.worldFogVolumePlaneCount = 0;
        gfxWorld.worldFogVolumePlanes = nullptr;

        // materialMemory is unused
        gfxWorld.materialMemoryCount = 0;
        gfxWorld.materialMemory = nullptr;

        // sunLight is overwritten by the game, just needs to be a valid pointer
        gfxWorld.sunLight = m_memory.Alloc<GfxLight>();
    }

    void GfxWorldLinker::loadGfxLights(GfxWorld& gfxWorld) const
    {
        // there must be 2 or more lights, first is the static light and second is the sun light
        gfxWorld.primaryLightCount = BSPGameConstants::BSP_DEFAULT_LIGHT_COUNT;
        gfxWorld.sunPrimaryLightIndex = BSPGameConstants::SUN_LIGHT_INDEX;

        gfxWorld.shadowGeom = m_memory.Alloc<GfxShadowGeometry>(gfxWorld.primaryLightCount);
        for (unsigned int lightIdx = 0; lightIdx < gfxWorld.primaryLightCount; lightIdx++)
        {
            gfxWorld.shadowGeom[lightIdx].smodelCount = 0;
            gfxWorld.shadowGeom[lightIdx].surfaceCount = 0;
            gfxWorld.shadowGeom[lightIdx].smodelIndex = nullptr;
            gfxWorld.shadowGeom[lightIdx].sortedSurfIndex = nullptr;
        }

        gfxWorld.lightRegion = m_memory.Alloc<GfxLightRegion>(gfxWorld.primaryLightCount);
        for (unsigned int lightIdx = 0; lightIdx < gfxWorld.primaryLightCount; lightIdx++)
        {
            gfxWorld.lightRegion[lightIdx].hullCount = 0;
            gfxWorld.lightRegion[lightIdx].hulls = nullptr;
        }

        unsigned int lightEntShadowVisSize = (gfxWorld.primaryLightCount - gfxWorld.sunPrimaryLightIndex - 1) * 8192;
        if (lightEntShadowVisSize != 0)
            gfxWorld.primaryLightEntityShadowVis = m_memory.Alloc<unsigned int>(lightEntShadowVisSize);
        else
            gfxWorld.primaryLightEntityShadowVis = nullptr;
    }

    void GfxWorldLinker::loadLightGrid(GfxWorld& gfxWorld) const
    {
        // there is almost no basis for the values in this code, they were chosen based on what looks correct when reverse engineering.

        // mins and maxs define the range that the lightgrid will work in.
        // unknown how these values are calculated, but the below values are larger
        // than official map values
        gfxWorld.lightGrid.mins[0] = 0;
        gfxWorld.lightGrid.mins[1] = 0;
        gfxWorld.lightGrid.mins[2] = 0;
        gfxWorld.lightGrid.maxs[0] = 200;
        gfxWorld.lightGrid.maxs[1] = 200;
        gfxWorld.lightGrid.maxs[2] = 50;

        gfxWorld.lightGrid.rowAxis = 0; // default value
        gfxWorld.lightGrid.colAxis = 1; // default value
        gfxWorld.lightGrid.sunPrimaryLightIndex = BSPGameConstants::SUN_LIGHT_INDEX;
        gfxWorld.lightGrid.offset = 0.0f; // default value

        // setting all rowDataStart indexes to 0 will always index the first row in rawRowData
        int rowDataStartSize = gfxWorld.lightGrid.maxs[gfxWorld.lightGrid.rowAxis] - gfxWorld.lightGrid.mins[gfxWorld.lightGrid.rowAxis] + 1;
        gfxWorld.lightGrid.rowDataStart = m_memory.Alloc<uint16_t>(rowDataStartSize);

        // Adding 0x0F so the lookup table will be 0x10 bytes in size
        gfxWorld.lightGrid.rawRowDataSize = static_cast<unsigned int>(sizeof(GfxLightGridRow) + 0x0F);
        GfxLightGridRow* row = static_cast<GfxLightGridRow*>(m_memory.AllocRaw(gfxWorld.lightGrid.rawRowDataSize));
        row->colStart = 0;
        row->colCount = 0x1000; // 0x1000 as this is large enough for all checks done by the game
        row->zStart = 0;
        row->zCount = 0xFF; // 0xFF as this is large enough for all checks done by the game, but small enough not to mess with other checks
        row->firstEntry = 0;
        for (int i = 0; i < 0x10; i++) // set the lookup table to all 0
            row->lookupTable[i] = 0;
        gfxWorld.lightGrid.rawRowData = reinterpret_cast<aligned_byte_pointer*>(row);

        // entries are looked up based on the lightgrid sample pos (given ingame) and the lightgrid lookup table
        gfxWorld.lightGrid.entryCount = 60000; // 60000 as it should be enough entries to be indexed by all lightgrid sample positions
        GfxLightGridEntry* entryArray = m_memory.Alloc<GfxLightGridEntry>(gfxWorld.lightGrid.entryCount);
        for (unsigned int i = 0; i < gfxWorld.lightGrid.entryCount; i++)
        {
            entryArray[i].colorsIndex = 0; // always index first colour
            entryArray[i].primaryLightIndex = BSPGameConstants::SUN_LIGHT_INDEX;
            entryArray[i].visibility = 0;
        }
        gfxWorld.lightGrid.entries = entryArray;

        // colours are looked up with a lightgrid entries colorsIndex
        gfxWorld.lightGrid.colorCount = 0x1000; // 0x1000 as it should be enough to hold every index
        gfxWorld.lightGrid.colors = m_memory.Alloc<GfxCompressedLightGridColors>(gfxWorld.lightGrid.colorCount);
        memset(gfxWorld.lightGrid.colors, BSPEditableConstants::LIGHTGRID_COLOUR, rowDataStartSize * sizeof(uint16_t));

        // we use the colours array instead of coeffs array
        gfxWorld.lightGrid.coeffCount = 0;
        gfxWorld.lightGrid.coeffs = nullptr;
        gfxWorld.lightGrid.skyGridVolumeCount = 0;
        gfxWorld.lightGrid.skyGridVolumes = nullptr;
    }

    void GfxWorldLinker::loadGfxCells(GfxWorld& gfxWorld) const
    {
        // Cells are basically data used to determine what can be seen and what cant be seen
        // Right now custom maps have no optimisation so there is only 1 cell
        int cellCount = 1;

        gfxWorld.dpvsPlanes.cellCount = cellCount;
        gfxWorld.cellBitsCount = ((cellCount + 127) >> 3) & 0x1FFFFFF0;

        int cellCasterBitsCount = cellCount * ((cellCount + 31) / 32);
        gfxWorld.cellCasterBits = m_memory.Alloc<unsigned int>(cellCasterBitsCount);

        int sceneEntCellBitsCount = cellCount * 512;
        gfxWorld.dpvsPlanes.sceneEntCellBits = m_memory.Alloc<unsigned int>(sceneEntCellBitsCount);

        gfxWorld.cells = m_memory.Alloc<GfxCell>(cellCount);
        gfxWorld.cells[0].portalCount = 0;
        gfxWorld.cells[0].portals = nullptr;
        gfxWorld.cells[0].mins.x = gfxWorld.mins.x;
        gfxWorld.cells[0].mins.y = gfxWorld.mins.y;
        gfxWorld.cells[0].mins.z = gfxWorld.mins.z;
        gfxWorld.cells[0].maxs.x = gfxWorld.maxs.x;
        gfxWorld.cells[0].maxs.y = gfxWorld.maxs.y;
        gfxWorld.cells[0].maxs.z = gfxWorld.maxs.z;

        // there is only 1 reflection probe
        gfxWorld.cells[0].reflectionProbeCount = 1;
        gfxWorld.cells[0].reflectionProbes = m_memory.Alloc<char>(gfxWorld.cells[0].reflectionProbeCount);
        gfxWorld.cells[0].reflectionProbes[0] = BSPEditableConstants::DEFAULT_SURFACE_REFLECTION_PROBE;

        // AABB trees are used to detect what should be rendered and what shouldn't
        // Just use the first AABB node to hold all models, no optimisation but all models/surfaces wil lbe drawn
        gfxWorld.cells[0].aabbTreeCount = 1;
        gfxWorld.cells[0].aabbTree = m_memory.Alloc<GfxAabbTree>(gfxWorld.cells[0].aabbTreeCount);
        gfxWorld.cells[0].aabbTree[0].childCount = 0;
        gfxWorld.cells[0].aabbTree[0].childrenOffset = 0;
        gfxWorld.cells[0].aabbTree[0].startSurfIndex = 0;
        gfxWorld.cells[0].aabbTree[0].surfaceCount = static_cast<uint16_t>(gfxWorld.surfaceCount);
        gfxWorld.cells[0].aabbTree[0].smodelIndexCount = static_cast<uint16_t>(gfxWorld.dpvs.smodelCount);
        gfxWorld.cells[0].aabbTree[0].smodelIndexes = m_memory.Alloc<unsigned short>(gfxWorld.dpvs.smodelCount);
        for (unsigned short smodelIdx = 0; smodelIdx < gfxWorld.dpvs.smodelCount; smodelIdx++)
        {
            gfxWorld.cells[0].aabbTree[0].smodelIndexes[smodelIdx] = smodelIdx;
        }
        gfxWorld.cells[0].aabbTree[0].mins.x = gfxWorld.mins.x;
        gfxWorld.cells[0].aabbTree[0].mins.y = gfxWorld.mins.y;
        gfxWorld.cells[0].aabbTree[0].mins.z = gfxWorld.mins.z;
        gfxWorld.cells[0].aabbTree[0].maxs.x = gfxWorld.maxs.x;
        gfxWorld.cells[0].aabbTree[0].maxs.y = gfxWorld.maxs.y;
        gfxWorld.cells[0].aabbTree[0].maxs.z = gfxWorld.maxs.z;

        // nodes have the struct mnode_t, and there must be at least 1 node (similar to BSP nodes)
        // Nodes mnode_t.cellIndex indexes gfxWorld->cells
        // and (mnode_t.cellIndex - (world->dpvsPlanes.cellCount + 1) indexes world->dpvsPlanes.planes
        // Use only one node as there is no optimisation in custom maps
        gfxWorld.nodeCount = 1;
        gfxWorld.dpvsPlanes.nodes = m_memory.Alloc<uint16_t>(gfxWorld.nodeCount);
        gfxWorld.dpvsPlanes.nodes[0] = 1; // nodes reference cells by index + 1

        // planes are overwritten by the clipmap loading code ingame
        gfxWorld.planeCount = 0;
        gfxWorld.dpvsPlanes.planes = nullptr;
    }

    void GfxWorldLinker::loadWorldBounds(GfxWorld& gfxWorld) const
    {
        gfxWorld.mins.x = 0.0f;
        gfxWorld.mins.y = 0.0f;
        gfxWorld.mins.z = 0.0f;
        gfxWorld.maxs.x = 0.0f;
        gfxWorld.maxs.y = 0.0f;
        gfxWorld.maxs.z = 0.0f;

        for (int surfIdx = 0; surfIdx < gfxWorld.surfaceCount; surfIdx++)
        {
            BSPUtil::updateAABB(gfxWorld.dpvs.surfaces[surfIdx].bounds[0], gfxWorld.dpvs.surfaces[surfIdx].bounds[1], gfxWorld.mins, gfxWorld.maxs);
        }
    }

    void GfxWorldLinker::loadModels(GfxWorld& gfxWorld) const
    {
        // Models (Submodels in the clipmap code) are used for the world and map ent collision (triggers, bomb zones, etc)
        // Right now there is only one submodel, the world sub model
        gfxWorld.modelCount = 1;
        gfxWorld.models = m_memory.Alloc<GfxBrushModel>(gfxWorld.modelCount);

        // first model is always the world model
        gfxWorld.models[0].startSurfIndex = 0;
        gfxWorld.models[0].surfaceCount = static_cast<unsigned int>(gfxWorld.surfaceCount);
        gfxWorld.models[0].bounds[0].x = gfxWorld.mins.x;
        gfxWorld.models[0].bounds[0].y = gfxWorld.mins.y;
        gfxWorld.models[0].bounds[0].z = gfxWorld.mins.z;
        gfxWorld.models[0].bounds[1].x = gfxWorld.maxs.x;
        gfxWorld.models[0].bounds[1].y = gfxWorld.maxs.y;
        gfxWorld.models[0].bounds[1].z = gfxWorld.maxs.z;
        memset(&gfxWorld.models[0].writable, 0, sizeof(GfxBrushModelWritable));

        // Other models aren't implemented yet
        // Code kept for future use
        // for (size_t i = 0; i < entityModelList.size(); i++)
        //{
        //    auto currEntModel = &gfxWorld->models[i + 1];
        //    entModelBounds currEntModelBounds = entityModelList[i];
        //
        //    currEntModel->startSurfIndex = 0;
        //    currEntModel->surfaceCount = -1; // -1 when it doesn't use map surfaces
        //    currEntModel->bounds[0].x = currEntModelBounds.mins.x;
        //    currEntModel->bounds[0].y = currEntModelBounds.mins.y;
        //    currEntModel->bounds[0].z = currEntModelBounds.mins.z;
        //    currEntModel->bounds[1].x = currEntModelBounds.maxs.x;
        //    currEntModel->bounds[1].y = currEntModelBounds.maxs.y;
        //    currEntModel->bounds[1].z = currEntModelBounds.maxs.z;
        //    memset(&gfxWorld->models[0].writable, 0, sizeof(GfxBrushModelWritable));
        //}
    }

    void GfxWorldLinker::loadSunData(GfxWorld& gfxWorld) const
    {
        // default values taken from mp_dig
        gfxWorld.sunParse.fogTransitionTime = 0.001f;
        gfxWorld.sunParse.name[0] = 0x00;

        gfxWorld.sunParse.initWorldSun->control = 0;
        gfxWorld.sunParse.initWorldSun->exposure = 2.5f;
        gfxWorld.sunParse.initWorldSun->angles.x = -29.0f;
        gfxWorld.sunParse.initWorldSun->angles.y = 254.0f;
        gfxWorld.sunParse.initWorldSun->angles.z = 0.0f;
        gfxWorld.sunParse.initWorldSun->sunCd.x = 1.0f;
        gfxWorld.sunParse.initWorldSun->sunCd.y = 0.89f;
        gfxWorld.sunParse.initWorldSun->sunCd.z = 0.69f;
        gfxWorld.sunParse.initWorldSun->sunCd.w = 13.5f;
        gfxWorld.sunParse.initWorldSun->ambientColor.x = 0.0f;
        gfxWorld.sunParse.initWorldSun->ambientColor.y = 0.0f;
        gfxWorld.sunParse.initWorldSun->ambientColor.z = 0.0f;
        gfxWorld.sunParse.initWorldSun->ambientColor.w = 0.0f;
        gfxWorld.sunParse.initWorldSun->skyColor.x = 0.0f;
        gfxWorld.sunParse.initWorldSun->skyColor.y = 0.0f;
        gfxWorld.sunParse.initWorldSun->skyColor.z = 0.0f;
        gfxWorld.sunParse.initWorldSun->skyColor.w = 0.0f;
        gfxWorld.sunParse.initWorldSun->sunCs.x = 0.0f;
        gfxWorld.sunParse.initWorldSun->sunCs.y = 0.0f;
        gfxWorld.sunParse.initWorldSun->sunCs.z = 0.0f;
        gfxWorld.sunParse.initWorldSun->sunCs.w = 0.0f;

        gfxWorld.sunParse.initWorldFog->baseDist = 150.0f;
        gfxWorld.sunParse.initWorldFog->baseHeight = -100.0f;
        gfxWorld.sunParse.initWorldFog->fogColor.x = 2.35f;
        gfxWorld.sunParse.initWorldFog->fogColor.y = 3.10f;
        gfxWorld.sunParse.initWorldFog->fogColor.z = 3.84f;
        gfxWorld.sunParse.initWorldFog->fogOpacity = 0.52f;
        gfxWorld.sunParse.initWorldFog->halfDist = 4450.f;
        gfxWorld.sunParse.initWorldFog->halfHeight = 2000.f;
        gfxWorld.sunParse.initWorldFog->sunFogColor.x = 5.27f;
        gfxWorld.sunParse.initWorldFog->sunFogColor.y = 4.73f;
        gfxWorld.sunParse.initWorldFog->sunFogColor.z = 3.88f;
        gfxWorld.sunParse.initWorldFog->sunFogInner = 0.0f;
        gfxWorld.sunParse.initWorldFog->sunFogOpacity = 0.67f;
        gfxWorld.sunParse.initWorldFog->sunFogOuter = 80.84f;
        gfxWorld.sunParse.initWorldFog->sunFogPitch = -29.0f;
        gfxWorld.sunParse.initWorldFog->sunFogYaw = 254.0f;
    }

    bool GfxWorldLinker::loadReflectionProbeData(GfxWorld& gfxWorld) const
    {
        gfxWorld.draw.reflectionProbeCount = 1;

        gfxWorld.draw.reflectionProbeTextures = m_memory.Alloc<GfxTexture>(gfxWorld.draw.reflectionProbeCount);

        // default values taken from mp_dig
        gfxWorld.draw.reflectionProbes = m_memory.Alloc<GfxReflectionProbe>(gfxWorld.draw.reflectionProbeCount);
        gfxWorld.draw.reflectionProbes[0].mipLodBias = -8.0;
        gfxWorld.draw.reflectionProbes[0].origin.x = 0.0f;
        gfxWorld.draw.reflectionProbes[0].origin.y = 0.0f;
        gfxWorld.draw.reflectionProbes[0].origin.z = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V0.x = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V0.y = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V0.z = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V0.w = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V1.x = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V1.y = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V1.z = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V1.w = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V2.x = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V2.y = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V2.z = 0.0f;
        gfxWorld.draw.reflectionProbes[0].lightingSH.V2.w = 0.0f;

        gfxWorld.draw.reflectionProbes[0].probeVolumeCount = 0;
        gfxWorld.draw.reflectionProbes[0].probeVolumes = nullptr;

        std::string probeImageName = "reflection_probe0";
        auto probeImageAsset = m_context.LoadDependency<AssetImage>(probeImageName);
        if (!probeImageAsset)
        {
            con::error("ERROR! unable to find reflection probe image {}!", probeImageName);
            return false;
        }
        gfxWorld.draw.reflectionProbes[0].reflectionImage = probeImageAsset->Asset();

        return true;
    }

    bool GfxWorldLinker::loadLightmapData(GfxWorld& gfxWorld) const
    {
        gfxWorld.draw.lightmapCount = 1;

        gfxWorld.draw.lightmapPrimaryTextures = m_memory.Alloc<GfxTexture>(gfxWorld.draw.lightmapCount);
        gfxWorld.draw.lightmapSecondaryTextures = m_memory.Alloc<GfxTexture>(gfxWorld.draw.lightmapCount);

        std::string secondaryTexture = "lightmap0_secondary";
        auto secondaryTextureAsset = m_context.LoadDependency<AssetImage>(secondaryTexture);
        if (!secondaryTextureAsset)
        {
            con::error("ERROR! unable to find lightmap image {}!", secondaryTexture);
            return false;
        }
        gfxWorld.draw.lightmaps = m_memory.Alloc<GfxLightmapArray>(gfxWorld.draw.lightmapCount);
        gfxWorld.draw.lightmaps[0].primary = nullptr; // always nullptr
        gfxWorld.draw.lightmaps[0].secondary = secondaryTextureAsset->Asset();

        return true;
    }

    void GfxWorldLinker::loadSkyBox(const BSPData& projInfo, GfxWorld& gfxWorld) const
    {
        const auto skyBoxName = std::format("skybox_{}", projInfo.name);
        gfxWorld.skyBoxModel = m_memory.Dup(skyBoxName.c_str());

        if (!m_context.LoadDependency<AssetXModel>(skyBoxName))
        {
            con::warn("WARN: Unable to load the skybox xmodel {}!", skyBoxName);
        }

        // default skybox values from mp_dig
        gfxWorld.skyDynIntensity.angle0 = 0.0f;
        gfxWorld.skyDynIntensity.angle1 = 0.0f;
        gfxWorld.skyDynIntensity.factor0 = 1.0f;
        gfxWorld.skyDynIntensity.factor1 = 1.0f;
    }

    void GfxWorldLinker::loadDynEntData(GfxWorld& gfxWorld) const
    {
        unsigned dynEntCount = 0u;
        gfxWorld.dpvsDyn.dynEntClientCount[0] = dynEntCount + 256; // the game allocs 256 empty dynents, as they may be used ingame
        gfxWorld.dpvsDyn.dynEntClientCount[1] = 0;

        // +100: there is a crash that happens when regdolls are created, and dynEntClientWordCount[0] is the issue.
        // Making the value much larger than required fixes it, but unsure what the root cause is
        gfxWorld.dpvsDyn.dynEntClientWordCount[0] = ((gfxWorld.dpvsDyn.dynEntClientCount[0] + 31) >> 5) + 100;
        gfxWorld.dpvsDyn.dynEntClientWordCount[1] = 0;
        gfxWorld.dpvsDyn.usageCount = 0;

        const auto dynEntCellBitsSize = gfxWorld.dpvsDyn.dynEntClientWordCount[0] * static_cast<unsigned>(gfxWorld.dpvsPlanes.cellCount);
        gfxWorld.dpvsDyn.dynEntCellBits[0] = m_memory.Alloc<unsigned int>(dynEntCellBitsSize);
        gfxWorld.dpvsDyn.dynEntCellBits[1] = nullptr;

        const auto dynEntVisData0Size = gfxWorld.dpvsDyn.dynEntClientWordCount[0] * 32u;
        gfxWorld.dpvsDyn.dynEntVisData[0][0] = m_memory.Alloc<char>(dynEntVisData0Size);
        gfxWorld.dpvsDyn.dynEntVisData[0][1] = m_memory.Alloc<char>(dynEntVisData0Size);
        gfxWorld.dpvsDyn.dynEntVisData[0][2] = m_memory.Alloc<char>(dynEntVisData0Size);
        gfxWorld.dpvsDyn.dynEntVisData[1][0] = nullptr;
        gfxWorld.dpvsDyn.dynEntVisData[1][1] = nullptr;
        gfxWorld.dpvsDyn.dynEntVisData[1][2] = nullptr;

        const auto dynEntShadowVisCount = gfxWorld.dpvsDyn.dynEntClientCount[0] * (gfxWorld.primaryLightCount - gfxWorld.sunPrimaryLightIndex - 1u);
        gfxWorld.primaryLightDynEntShadowVis[0] = m_memory.Alloc<unsigned int>(dynEntShadowVisCount);
        gfxWorld.primaryLightDynEntShadowVis[1] = nullptr;

        gfxWorld.sceneDynModel = m_memory.Alloc<GfxSceneDynModel>(gfxWorld.dpvsDyn.dynEntClientCount[0]);
        gfxWorld.sceneDynBrush = nullptr;
    }

    bool GfxWorldLinker::loadOutdoors(GfxWorld& gfxWorld) const
    {
        const auto xRecip = 1.0f / (gfxWorld.maxs.x - gfxWorld.mins.x);
        const auto xScale = -(xRecip * gfxWorld.mins.x);

        const auto yRecip = 1.0f / (gfxWorld.maxs.y - gfxWorld.mins.y);
        const auto yScale = -(yRecip * gfxWorld.mins.y);

        const auto zRecip = 1.0f / (gfxWorld.maxs.z - gfxWorld.mins.z);
        const auto zScale = -(zRecip * gfxWorld.mins.z);

        memset(gfxWorld.outdoorLookupMatrix, 0, sizeof(gfxWorld.outdoorLookupMatrix));

        gfxWorld.outdoorLookupMatrix[0].x = xRecip;
        gfxWorld.outdoorLookupMatrix[1].y = yRecip;
        gfxWorld.outdoorLookupMatrix[2].z = zRecip;
        gfxWorld.outdoorLookupMatrix[3].x = xScale;
        gfxWorld.outdoorLookupMatrix[3].y = yScale;
        gfxWorld.outdoorLookupMatrix[3].z = zScale;
        gfxWorld.outdoorLookupMatrix[3].w = 1.0f;

        const auto outdoorImageAsset = m_context.LoadDependency<AssetImage>("$outdoor");
        if (!outdoorImageAsset)
        {
            con::error("ERROR! unable to find outdoor image $outdoor!");
            return false;
        }
        gfxWorld.outdoorImage = outdoorImageAsset->Asset();

        return true;
    }

    GfxWorld* GfxWorldLinker::linkGfxWorld(const BSPData& bsp) const
    {
        GfxWorld* gfxWorld = m_memory.Alloc<GfxWorld>();
        gfxWorld->baseName = m_memory.Dup(bsp.name.c_str());
        gfxWorld->name = m_memory.Dup(bsp.bspName.c_str());

        // Default values taken from official maps
        gfxWorld->lightingFlags = 0;
        gfxWorld->lightingQuality = 4096;

        cleanGfxWorld(*gfxWorld);

        if (!loadMapSurfaces(bsp, *gfxWorld))
            return nullptr;

        loadXModels(bsp, *gfxWorld);

        if (!loadLightmapData(*gfxWorld))
            return nullptr;

        loadSkyBox(bsp, *gfxWorld);

        if (!loadReflectionProbeData(*gfxWorld))
            return nullptr;

        // world bounds are based on loaded surface mins/maxs
        loadWorldBounds(*gfxWorld);

        if (!loadOutdoors(*gfxWorld))
            return nullptr;

        // gfx cells depend on surface/smodel count
        loadGfxCells(*gfxWorld);

        loadLightGrid(*gfxWorld);

        loadGfxLights(*gfxWorld);

        loadModels(*gfxWorld);

        loadSunData(*gfxWorld);

        loadDynEntData(*gfxWorld);

        return gfxWorld;
    }
} // namespace BSP
