#include "GfxWorldLinker.h"
#include "../BSPUtil.h"
#include "Utils/Pack.h"

namespace BSP
{
    GfxWorldLinker::GfxWorldLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
        m_search_path(searchPath),
        m_context(context)
    {
    }

    // TODO vd1:
    //	used for UVs of sub-textures, when it is set to empty all of them turn a blank colour
    //	could fix by removing sub textures or figure out how they are created and redo that
    //	its not an important issue though
    void GfxWorldLinker::overwriteDrawData(BSPData* projInfo, GfxWorld* gfxWorld)
    {
        int vertexCount = projInfo->gfxWorld.vertices.size();

        gfxWorld->draw.vertexCount = vertexCount;
        gfxWorld->draw.vertexDataSize0 = vertexCount * sizeof(GfxPackedWorldVertex);
        GfxPackedWorldVertex* vertexBuffer = m_memory.Alloc<GfxPackedWorldVertex>(vertexCount);
        for (int i = 0; i < vertexCount; i++)
        {
            BSPVertex* WorldVertex = &projInfo->gfxWorld.vertices[i];
            GfxPackedWorldVertex* GfxVertex = &vertexBuffer[i];

            GfxVertex->xyz = BSPUtil::convertToBO2Coords(WorldVertex->pos);
            //GfxVertex->xyz = WorldVertex->pos;

            GfxVertex->color.packed = pack32::Vec4PackGfxColor(WorldVertex->color.v);

            GfxVertex->texCoord.packed = pack32::Vec2PackTexCoordsUV(WorldVertex->texCoord.v);

            GfxVertex->normal.packed = pack32::Vec3PackUnitVecThirdBased(BSPUtil::convertToBO2Coords(WorldVertex->normal).v);
            //GfxVertex->normal.packed = pack32::Vec3PackUnitVecThirdBased(WorldVertex->normal.v);

            GfxVertex->tangent.packed = pack32::Vec3PackUnitVecThirdBased(BSPUtil::convertToBO2Coords(WorldVertex->tangent).v);
            //GfxVertex->tangent.packed = pack32::Vec3PackUnitVecThirdBased(WorldVertex->tangent.v);

            // unknown use variables
            // binormalSign may be bitangent of the vertex
            // lmapCoord may be the lightmap coordinate of the vertex
            GfxVertex->binormalSign = 0.0f;
            GfxVertex->lmapCoord.packed = 0;
        }
        gfxWorld->draw.vd0.data = (char*)vertexBuffer;

        // we don't use vd1 but still needs to be initialised
        // the data type varies and 0x20 is enough for all types
        gfxWorld->draw.vertexDataSize1 = 0x20;
        gfxWorld->draw.vd1.data = m_memory.Alloc<char>(gfxWorld->draw.vertexDataSize1);
        memset(gfxWorld->draw.vd1.data, 0, gfxWorld->draw.vertexDataSize1);

        int indexCount = projInfo->gfxWorld.indices.size();
        assert(indexCount % 3 == 0);
        gfxWorld->draw.indexCount = indexCount;
        gfxWorld->draw.indices = m_memory.Alloc<uint16_t>(indexCount);
        for (int i = 0; i < indexCount; i += 3)
        {
            // the editor orders their vertices opposite to bo2, so its converted here
            gfxWorld->draw.indices[i + 2] = projInfo->gfxWorld.indices[i + 0];
            gfxWorld->draw.indices[i + 1] = projInfo->gfxWorld.indices[i + 1];
            gfxWorld->draw.indices[i + 0] = projInfo->gfxWorld.indices[i + 2];
        }
    }

    bool GfxWorldLinker::overwriteMapSurfaces(BSPData* projInfo, GfxWorld* gfxWorld)
    {
        overwriteDrawData(projInfo, gfxWorld);

        unsigned int surfaceCount = projInfo->gfxWorld.surfaces.size();
        gfxWorld->surfaceCount = surfaceCount;
        gfxWorld->dpvs.staticSurfaceCount = surfaceCount;
        gfxWorld->dpvs.surfaces = m_memory.Alloc<GfxSurface>(surfaceCount);
        for (unsigned int i = 0; i < surfaceCount; i++)
        {
            auto currSurface = &gfxWorld->dpvs.surfaces[i];
            auto objSurface = &projInfo->gfxWorld.surfaces[i];

            currSurface->primaryLightIndex = BSPEditableConstants::DEFAULT_SURFACE_LIGHT;
            currSurface->lightmapIndex = BSPEditableConstants::DEFAULT_SURFACE_LIGHTMAP;
            currSurface->reflectionProbeIndex = BSPEditableConstants::DEFAULT_SURFACE_REFLECTION_PROBE;
            currSurface->flags = BSPEditableConstants::DEFAULT_SURFACE_FLAGS;

            currSurface->tris.triCount = objSurface->triCount;
            currSurface->tris.baseIndex = objSurface->indexOfFirstIndex;

            currSurface->tris.vertexDataOffset0 = objSurface->indexOfFirstVertex * sizeof(GfxPackedWorldVertex);
            currSurface->tris.vertexDataOffset1 = 0;

            std::string surfMaterialName;
            switch (objSurface->material.materialType)
            {
            case MATERIAL_TYPE_TEXTURE:
                surfMaterialName = objSurface->material.materialName;
                break;

            case MATERIAL_TYPE_COLOUR:
            case MATERIAL_TYPE_EMPTY:
                surfMaterialName = BSPLinkingConstants::COLOR_ONLY_IMAGE_NAME;
                break;

            default:
                assert(false);
            }

            auto surfMaterialAsset = m_context.LoadDependency<AssetMaterial>(surfMaterialName);
            if (surfMaterialAsset == NULL)
            {
                std::string missingImageName = BSPLinkingConstants::MISSING_IMAGE_NAME;
                surfMaterialAsset = m_context.LoadDependency<AssetMaterial>(missingImageName);
                if (surfMaterialAsset == NULL)
                {
                    con::error("unable to find the missing image texture!");
                    return false;
                }
            }
            currSurface->material = surfMaterialAsset->Asset();

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
                BSPUtil::calcNewBoundsWithPoint(&firstVert[vertIndex].xyz, &currSurface->bounds[0], &currSurface->bounds[1]);
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
        gfxWorld->dpvs.sortedSurfIndex = m_memory.Alloc<uint16_t>(surfaceCount);
        for (unsigned int i = 0; i < surfaceCount; i++)
        {
            gfxWorld->dpvs.sortedSurfIndex[i] = i;
        }

        // surface materials are written to by the game
        gfxWorld->dpvs.surfaceMaterials = m_memory.Alloc<GfxDrawSurf_align4>(surfaceCount);
        memset(gfxWorld->dpvs.surfaceMaterials, 0, sizeof(GfxDrawSurf_align4) * surfaceCount);

        // set all surface types to lit opaque
        gfxWorld->dpvs.litSurfsBegin = 0;
        gfxWorld->dpvs.litSurfsEnd = surfaceCount;
        gfxWorld->dpvs.emissiveOpaqueSurfsBegin = surfaceCount;
        gfxWorld->dpvs.emissiveOpaqueSurfsEnd = surfaceCount;
        gfxWorld->dpvs.emissiveTransSurfsBegin = surfaceCount;
        gfxWorld->dpvs.emissiveTransSurfsEnd = surfaceCount;
        gfxWorld->dpvs.litTransSurfsBegin = surfaceCount;
        gfxWorld->dpvs.litTransSurfsEnd = surfaceCount;

        // visdata is written to by the game
        // all visdata is alligned by 128
        unsigned int allignedSurfaceCount = BSPUtil::allignBy128(surfaceCount);
        gfxWorld->dpvs.surfaceVisDataCount = allignedSurfaceCount;
        gfxWorld->dpvs.surfaceVisData[0] = m_memory.Alloc<char>(allignedSurfaceCount);
        gfxWorld->dpvs.surfaceVisData[1] = m_memory.Alloc<char>(allignedSurfaceCount);
        gfxWorld->dpvs.surfaceVisData[2] = m_memory.Alloc<char>(allignedSurfaceCount);
        gfxWorld->dpvs.surfaceVisDataCameraSaved = m_memory.Alloc<char>(allignedSurfaceCount);
        gfxWorld->dpvs.surfaceCastsShadow = m_memory.Alloc<char>(allignedSurfaceCount);
        gfxWorld->dpvs.surfaceCastsSunShadow = m_memory.Alloc<char>(allignedSurfaceCount);
        memset(gfxWorld->dpvs.surfaceVisData[0], 0, allignedSurfaceCount);
        memset(gfxWorld->dpvs.surfaceVisData[1], 0, allignedSurfaceCount);
        memset(gfxWorld->dpvs.surfaceVisData[2], 0, allignedSurfaceCount);
        memset(gfxWorld->dpvs.surfaceVisDataCameraSaved, 0, allignedSurfaceCount);
        memset(gfxWorld->dpvs.surfaceCastsShadow, 0, allignedSurfaceCount);
        memset(gfxWorld->dpvs.surfaceCastsSunShadow, 0, allignedSurfaceCount);

        return true;
    }

    void GfxWorldLinker::overwriteMapSModels(BSPData* projInfo, GfxWorld* gfxWorld)
    {
        /*
        Models are unsupported right now
        Code is left in in case it is supported later on
        */
        //unsigned int modelCount = projInfo->modelCount;
        //gfxWorld->dpvs.smodelCount = modelCount;
        //gfxWorld->dpvs.smodelInsts = new GfxStaticModelInst[modelCount];
        //gfxWorld->dpvs.smodelDrawInsts = new GfxStaticModelDrawInst[modelCount];
        //
        //for (unsigned int i = 0; i < modelCount; i++)
        //{
        //        auto currModel = &gfxWorld->dpvs.smodelDrawInsts[i];
        //        auto currModelInst = &gfxWorld->dpvs.smodelInsts[i];
        //        customMapModel* inModel = &projInfo->models[i];
        //
        //        auto xModelAsset = m_context.LoadDependency<AssetXModel>(inModel->name);
        //        if (xModelAsset == NULL)
        //        {
        //            printf("XModel %s not found!\n", inModel->name.c_str());
        //            currModel->model = NULL;
        //        }
        //        else
        //            currModel->model = (XModel*)xModelAsset->Asset();
        //
        //        currModel->placement.origin.x = inModel->origin.x;
        //        currModel->placement.origin.y = inModel->origin.y;
        //        currModel->placement.origin.z = inModel->origin.z;
        //        currModel->placement.origin = BSPUtil::convertToBO2Coords(currModel->placement.origin);
        //        currModel->placement.scale = inModel->scale;
        //
        //        BSPUtil::convertAnglesToAxis(&inModel->rotation, currModel->placement.axis);
        //
        //        // mins and maxs are calculated in world space not local space
        //        // TODO: this does not account for model rotation or scale
        //        currModelInst->mins.x = currModel->model->mins.x + currModel->placement.origin.x;
        //        currModelInst->mins.y = currModel->model->mins.y + currModel->placement.origin.y;
        //        currModelInst->mins.z = currModel->model->mins.z + currModel->placement.origin.z;
        //        currModelInst->maxs.x = currModel->model->maxs.x + currModel->placement.origin.x;
        //        currModelInst->maxs.y = currModel->model->maxs.y + currModel->placement.origin.y;
        //        currModelInst->maxs.z = currModel->model->maxs.z + currModel->placement.origin.z;
        //
        //        currModel->cullDist = DEFAULT_SMODEL_CULL_DIST;
        //        currModel->flags = DEFAULT_SMODEL_FLAGS;
        //        currModel->primaryLightIndex = DEFAULT_SMODEL_LIGHT;
        //        currModel->reflectionProbeIndex = DEFAULT_SMODEL_REFLECTION_PROBE;
        //
        //        // unknown use / unused
        //        currModel->smid = i;
        //        memset(&currModel->lightingSH, 0, sizeof(GfxLightingSHQuantized));
        //        currModel->invScaleSq = 0.0f;
        //        currModel->lightingHandle = 0;
        //        currModel->colorsIndex = 0;
        //        currModel->visibility = 0;
        //
        //        // setting these to NULL makes any static/baked lighting go black when not rendered by real-time lighting or in a shadow
        //        // TODO: calculate lighting and store it here
        //        currModel->lmapVertexInfo[0].numLmapVertexColors = 0;
        //        currModel->lmapVertexInfo[0].lmapVertexColors = NULL;
        //        currModel->lmapVertexInfo[1].numLmapVertexColors = 0;
        //        currModel->lmapVertexInfo[1].lmapVertexColors = NULL;
        //        currModel->lmapVertexInfo[2].numLmapVertexColors = 0;
        //        currModel->lmapVertexInfo[2].lmapVertexColors = NULL;
        //        currModel->lmapVertexInfo[3].numLmapVertexColors = 0;
        //        currModel->lmapVertexInfo[3].lmapVertexColors = NULL;
        //}

        unsigned int modelCount = 0;
        gfxWorld->dpvs.smodelCount = modelCount;
        gfxWorld->dpvs.smodelInsts = m_memory.Alloc<GfxStaticModelInst>(modelCount);
        gfxWorld->dpvs.smodelDrawInsts = m_memory.Alloc<GfxStaticModelDrawInst>(modelCount);

        // visdata is written to by the game
        // all visdata is alligned by 128
        int allignedModelCount = BSPUtil::allignBy128(modelCount);
        gfxWorld->dpvs.smodelVisDataCount = allignedModelCount;
        gfxWorld->dpvs.smodelVisData[0] = m_memory.Alloc<char>(allignedModelCount);
        gfxWorld->dpvs.smodelVisData[1] = m_memory.Alloc<char>(allignedModelCount);
        gfxWorld->dpvs.smodelVisData[2] = m_memory.Alloc<char>(allignedModelCount);
        gfxWorld->dpvs.smodelVisDataCameraSaved = m_memory.Alloc<char>(allignedModelCount);
        gfxWorld->dpvs.smodelCastsShadow = m_memory.Alloc<char>(allignedModelCount);
        memset(gfxWorld->dpvs.smodelVisData[0], 0, allignedModelCount);
        memset(gfxWorld->dpvs.smodelVisData[1], 0, allignedModelCount);
        memset(gfxWorld->dpvs.smodelVisData[2], 0, allignedModelCount);
        memset(gfxWorld->dpvs.smodelVisDataCameraSaved, 0, allignedModelCount);
        memset(gfxWorld->dpvs.smodelCastsShadow, 0, allignedModelCount);
        for (unsigned int i = 0; i < modelCount; i++)
        {
            if ((gfxWorld->dpvs.smodelDrawInsts[i].flags & STATIC_MODEL_FLAG_NO_SHADOW) == 0)
                gfxWorld->dpvs.smodelCastsShadow[i] = 1;
            else
                gfxWorld->dpvs.smodelCastsShadow[i] = 0;
        }

        // official maps set this to 0
        gfxWorld->dpvs.usageCount = 0;
    }

    void GfxWorldLinker::cleanGfxWorld(GfxWorld* gfxWorld)
    {
        // checksum is generated by the game
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
        gfxWorld->sunLight = m_memory.Alloc<GfxLight>();
        memset(gfxWorld->sunLight, 0, sizeof(GfxLight));
    }

    void GfxWorldLinker::overwriteGfxLights(GfxWorld* gfxWorld)
    {
        // there must be 2 or more lights, first is the default light and second is the sun
        gfxWorld->primaryLightCount = 2;
        gfxWorld->sunPrimaryLightIndex = BSPGameConstants::SUN_LIGHT_INDEX;

        gfxWorld->shadowGeom = m_memory.Alloc<GfxShadowGeometry>(gfxWorld->primaryLightCount);
        for (unsigned int i = 0; i < gfxWorld->primaryLightCount; i++)
        {
            gfxWorld->shadowGeom[i].smodelCount = 0;
            gfxWorld->shadowGeom[i].surfaceCount = 0;
            gfxWorld->shadowGeom[i].smodelIndex = NULL;
            gfxWorld->shadowGeom[i].sortedSurfIndex = NULL;
        }

        gfxWorld->lightRegion = m_memory.Alloc<GfxLightRegion>(gfxWorld->primaryLightCount);
        for (unsigned int i = 0; i < gfxWorld->primaryLightCount; i++)
        {
            gfxWorld->lightRegion[i].hullCount = 0;
            gfxWorld->lightRegion[i].hulls = NULL;
        }

        int lightEntShadowVisSize = (gfxWorld->primaryLightCount - gfxWorld->sunPrimaryLightIndex - 1) * 8192;
        if (lightEntShadowVisSize != 0)
        {
            gfxWorld->primaryLightEntityShadowVis = m_memory.Alloc<unsigned int>(lightEntShadowVisSize);
            memset(gfxWorld->primaryLightEntityShadowVis, 0, lightEntShadowVisSize * sizeof(unsigned int));
        }
        else
        {
            gfxWorld->primaryLightEntityShadowVis = NULL;
        }
    }

    // the lightgrid is used to light models in a dynamic way and is precomputed
    void GfxWorldLinker::overwriteLightGrid(GfxWorld* gfxWorld)
    {
        // there is almost no basis for the values in this code, i chose them based on what feels right and what i could see when RE.
        // it works and that is all thats needed :)

        // mins and maxs define the range that the lightgrid will work in
        // idk how these values are calculated, but the below values are larger
        // than official map values
        gfxWorld->lightGrid.mins[0] = 0;
        gfxWorld->lightGrid.mins[1] = 0;
        gfxWorld->lightGrid.mins[2] = 0;
        gfxWorld->lightGrid.maxs[0] = 200;
        gfxWorld->lightGrid.maxs[1] = 200;
        gfxWorld->lightGrid.maxs[2] = 50;

        gfxWorld->lightGrid.rowAxis = 0;              // default value
        gfxWorld->lightGrid.colAxis = 1;              // default value
        gfxWorld->lightGrid.sunPrimaryLightIndex = BSPGameConstants::SUN_LIGHT_INDEX;
        gfxWorld->lightGrid.offset = 0.0f; // default value

        // this will make the lookup into rawRowData always return the first row
        int rowDataStartSize = gfxWorld->lightGrid.maxs[gfxWorld->lightGrid.rowAxis] - gfxWorld->lightGrid.mins[gfxWorld->lightGrid.rowAxis] + 1;
        gfxWorld->lightGrid.rowDataStart = m_memory.Alloc<uint16_t>(rowDataStartSize);
        memset(gfxWorld->lightGrid.rowDataStart, 0, rowDataStartSize * sizeof(uint16_t));

        gfxWorld->lightGrid.rawRowDataSize = sizeof(GfxLightGridRow) + 0x10;
        GfxLightGridRow* row = (GfxLightGridRow*)m_memory.AllocRaw(sizeof(GfxLightGridRow) + 0x10);
        row->colStart = 0;
        row->colCount = 0x1000; // 0x1000 as this is large enough for all checks done by the game
        row->zStart = 0;
        row->zCount = 0xFF; // 0xFF as this is large enough for all checks done by the game, but small enough not to mess with other checks
        row->firstEntry = 0;
        for (int i = 0; i < 0x11; i++) // set the lookup table to all 0
        {
            row->lookupTable[i] = 0;
        }
        gfxWorld->lightGrid.rawRowData = (aligned_byte_pointer*)row;

        // entries are looked up based on the lightgrid sample pos (given ingame) and the lightgrid lookup table
        gfxWorld->lightGrid.entryCount = 60000; // 60000 as it should be enough entries to be indexed by all lightgrid data
        GfxLightGridEntry* entryArray = m_memory.Alloc<GfxLightGridEntry>(gfxWorld->lightGrid.entryCount);
        for (unsigned int i = 0; i < gfxWorld->lightGrid.entryCount; i++)
        {
            entryArray[i].colorsIndex = 0; // always index first colour
            entryArray[i].primaryLightIndex = BSPGameConstants::SUN_LIGHT_INDEX;
            entryArray[i].visibility = 0;
        }
        gfxWorld->lightGrid.entries = entryArray;

        // colours are looked up by an entries colourindex
        gfxWorld->lightGrid.colorCount = 0x1000; //0x1000 as it should be enough to hold every index
        gfxWorld->lightGrid.colors = m_memory.Alloc<GfxCompressedLightGridColors>(gfxWorld->lightGrid.colorCount);
        memset(gfxWorld->lightGrid.colors, BSPEditableConstants::LIGHTGRID_COLOUR, rowDataStartSize * sizeof(uint16_t));

        // we use the colours array instead of coeffs array
        gfxWorld->lightGrid.coeffCount = 0;
        gfxWorld->lightGrid.coeffs = NULL;
        gfxWorld->lightGrid.skyGridVolumeCount = 0;
        gfxWorld->lightGrid.skyGridVolumes = NULL;
    }

    void GfxWorldLinker::updateGfxCells(GfxWorld* gfxWorld)
    {
        // Cells are basically data used to determine what can be seen and what cant be seen
        // Right now custom maps have no optimisation so there is only 1 cell
        int cellCount = 1;

        gfxWorld->cellBitsCount = ((cellCount + 127) >> 3) & 0x1FFFFFF0;

        int cellCasterBitsCount = cellCount * ((cellCount + 31) / 32);
        gfxWorld->cellCasterBits = m_memory.Alloc<unsigned int>(cellCasterBitsCount);
        memset(gfxWorld->cellCasterBits, 0x00, cellCasterBitsCount * sizeof(unsigned int));

        gfxWorld->cells = m_memory.Alloc<GfxCell>(cellCount);
        gfxWorld->cells[0].portalCount = 0;
        gfxWorld->cells[0].portals = NULL;
        gfxWorld->cells[0].mins.x = gfxWorld->mins.x;
        gfxWorld->cells[0].mins.y = gfxWorld->mins.y;
        gfxWorld->cells[0].mins.z = gfxWorld->mins.z;
        gfxWorld->cells[0].maxs.x = gfxWorld->maxs.x;
        gfxWorld->cells[0].maxs.y = gfxWorld->maxs.y;
        gfxWorld->cells[0].maxs.z = gfxWorld->maxs.z;

        // there is only 1 reflection probe
        gfxWorld->cells[0].reflectionProbeCount = 1;
        char* reflectionProbeIndexes = m_memory.Alloc<char>(gfxWorld->cells[0].reflectionProbeCount);
        reflectionProbeIndexes[0] = BSPEditableConstants::DEFAULT_SURFACE_REFLECTION_PROBE;
        gfxWorld->cells[0].reflectionProbes = reflectionProbeIndexes;

        // AABB trees are used to detect what should be rendered and what shouldn't
        // Just use the first AABB node to hold all models, no optimisation but all models/surfaces wil lbe drawn
        gfxWorld->cells[0].aabbTreeCount = 1;
        gfxWorld->cells[0].aabbTree = m_memory.Alloc<GfxAabbTree>(gfxWorld->cells[0].aabbTreeCount);
        gfxWorld->cells[0].aabbTree[0].childCount = 0;
        gfxWorld->cells[0].aabbTree[0].childrenOffset = 0;
        gfxWorld->cells[0].aabbTree[0].startSurfIndex = 0;
        gfxWorld->cells[0].aabbTree[0].surfaceCount = gfxWorld->surfaceCount;
        gfxWorld->cells[0].aabbTree[0].smodelIndexCount = gfxWorld->dpvs.smodelCount;
        gfxWorld->cells[0].aabbTree[0].smodelIndexes = m_memory.Alloc<unsigned short>(gfxWorld->dpvs.smodelCount);
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
        gfxWorld->dpvsPlanes.sceneEntCellBits = m_memory.Alloc<unsigned int>(sceneEntCellBitsCount);
        memset(gfxWorld->dpvsPlanes.sceneEntCellBits, 0x00, sceneEntCellBitsCount * sizeof(unsigned int));

        // nodes have the struct mnode_t, and there must be at least 1 node
        // Nodes mnode_t.cellIndex indexes gfxWorld->cells
        // and (mnode_t.cellIndex - (world->dpvsPlanes.cellCount + 1) indexes world->dpvsPlanes.planes
        gfxWorld->nodeCount = 1;
        gfxWorld->dpvsPlanes.nodes = m_memory.Alloc<uint16_t>(gfxWorld->nodeCount);
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
            BSPUtil::calcNewBounds(&gfxWorld->dpvs.surfaces[i].bounds[0], &gfxWorld->dpvs.surfaces[i].bounds[1], &gfxWorld->mins, &gfxWorld->maxs);
        }
    }

    void GfxWorldLinker::overwriteModels(GfxWorld* gfxWorld)
    {
        // Models (Submodels in the clipmap code) are used for the world and map ent collision (triggers, bomb zones, etc)
        // Right now there is only one submodel, the world sub model
        gfxWorld->modelCount = 1;
        gfxWorld->models = m_memory.Alloc<GfxBrushModel>(gfxWorld->modelCount);

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

        // Other models aren't implemented yet
        // Code kept for future use
        //for (size_t i = 0; i < entityModelList.size(); i++)
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

    bool GfxWorldLinker::updateReflectionProbeData(GfxWorld* gfxWorld)
    {
        gfxWorld->draw.reflectionProbeCount = 1;

        gfxWorld->draw.reflectionProbeTextures = m_memory.Alloc<GfxTexture>(gfxWorld->draw.reflectionProbeCount);
        memset(gfxWorld->draw.reflectionProbeTextures, 0, sizeof(GfxTexture) * gfxWorld->draw.reflectionProbeCount);

        gfxWorld->draw.reflectionProbes = m_memory.Alloc<GfxReflectionProbe>(gfxWorld->draw.reflectionProbeCount);

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
            con::error("ERROR! unable to find reflection probe image {}!", probeImageName);
            return false;
        }
        gfxWorld->draw.reflectionProbes[0].reflectionImage = probeImageAsset->Asset();

        return true;
    }

    bool GfxWorldLinker::overwriteLightmapData(GfxWorld* gfxWorld)
    {
        gfxWorld->draw.lightmapCount = 1;

        gfxWorld->draw.lightmapPrimaryTextures = m_memory.Alloc<GfxTexture>(gfxWorld->draw.lightmapCount);
        gfxWorld->draw.lightmapSecondaryTextures = m_memory.Alloc<GfxTexture>(gfxWorld->draw.lightmapCount);
        gfxWorld->draw.lightmaps = m_memory.Alloc<GfxLightmapArray>(gfxWorld->draw.lightmapCount);

        // always set to 0
        memset(gfxWorld->draw.lightmapPrimaryTextures, 0, sizeof(GfxTexture) * gfxWorld->draw.lightmapCount);
        memset(gfxWorld->draw.lightmapSecondaryTextures, 0, sizeof(GfxTexture) * gfxWorld->draw.lightmapCount);

        std::string secondaryTexture = "*lightmap0_secondary";
        auto secondaryTextureAsset = m_context.LoadDependency<AssetImage>(secondaryTexture);
        if (secondaryTextureAsset == NULL)
        {
            con::error("ERROR! unable to find lightmap image {}!", secondaryTexture);
            return false;
        }
        gfxWorld->draw.lightmaps[0].primary = NULL; // always NULL
        gfxWorld->draw.lightmaps[0].secondary = secondaryTextureAsset->Asset();

        return true;
    }

    void GfxWorldLinker::overwriteSkyBox(BSPData* projInfo, GfxWorld* gfxWorld)
    {
        std::string skyBoxName = "skybox_" + projInfo->name;
        gfxWorld->skyBoxModel = m_memory.Dup(skyBoxName.c_str());

        if (m_context.LoadDependency<AssetXModel>(skyBoxName) == NULL)
        {
            con::warn("WARN: Unable to load the skybox xmodel {}!", skyBoxName);
        }

        // default skybox values from mp_dig
        gfxWorld->skyDynIntensity.angle0 = 0.0f;
        gfxWorld->skyDynIntensity.angle1 = 0.0f;
        gfxWorld->skyDynIntensity.factor0 = 1.0f;
        gfxWorld->skyDynIntensity.factor1 = 1.0f;
    }

    void GfxWorldLinker::updateDynEntData(GfxWorld* gfxWorld)
    {
        int dynEntCount = 0;
        gfxWorld->dpvsDyn.dynEntClientCount[0] = dynEntCount + 256; // the game allocs 256 empty dynents, as they may be used ingame
        gfxWorld->dpvsDyn.dynEntClientCount[1] = 0;

        // +100: there is a crash that happens when regdolls are created, and dynEntClientWordCount[0] is the issue. 
        // Making the value much larger than required fixes it, but idk what the root cause is
        gfxWorld->dpvsDyn.dynEntClientWordCount[0] = ((gfxWorld->dpvsDyn.dynEntClientCount[0] + 31) >> 5) + 100;
        gfxWorld->dpvsDyn.dynEntClientWordCount[1] = 0;
        gfxWorld->dpvsDyn.usageCount = 0;

        int dynEntCellBitsSize = gfxWorld->dpvsDyn.dynEntClientWordCount[0] * gfxWorld->dpvsPlanes.cellCount;
        gfxWorld->dpvsDyn.dynEntCellBits[0] = m_memory.Alloc<unsigned int>(dynEntCellBitsSize);
        gfxWorld->dpvsDyn.dynEntCellBits[1] = NULL;
        memset(gfxWorld->dpvsDyn.dynEntCellBits[0], 0, sizeof(unsigned int) * dynEntCellBitsSize);

        int dynEntVisData0Size = gfxWorld->dpvsDyn.dynEntClientWordCount[0] * 32;
        gfxWorld->dpvsDyn.dynEntVisData[0][0] = m_memory.Alloc<char>(dynEntVisData0Size);
        gfxWorld->dpvsDyn.dynEntVisData[0][1] = m_memory.Alloc<char>(dynEntVisData0Size);
        gfxWorld->dpvsDyn.dynEntVisData[0][2] = m_memory.Alloc<char>(dynEntVisData0Size);
        gfxWorld->dpvsDyn.dynEntVisData[1][0] = NULL;
        gfxWorld->dpvsDyn.dynEntVisData[1][1] = NULL;
        gfxWorld->dpvsDyn.dynEntVisData[1][2] = NULL;
        memset(gfxWorld->dpvsDyn.dynEntVisData[0][0], 0, dynEntVisData0Size);
        memset(gfxWorld->dpvsDyn.dynEntVisData[0][1], 0, dynEntVisData0Size);
        memset(gfxWorld->dpvsDyn.dynEntVisData[0][2], 0, dynEntVisData0Size);

        int dynEntShadowVisCount = gfxWorld->dpvsDyn.dynEntClientCount[0] * (gfxWorld->primaryLightCount - gfxWorld->sunPrimaryLightIndex - 1);
        gfxWorld->primaryLightDynEntShadowVis[0] = m_memory.Alloc<unsigned int>(dynEntShadowVisCount);
        gfxWorld->primaryLightDynEntShadowVis[1] = NULL;
        memset(gfxWorld->primaryLightDynEntShadowVis[0], 0, sizeof(unsigned int) * dynEntShadowVisCount);

        gfxWorld->sceneDynModel = m_memory.Alloc<GfxSceneDynModel>(gfxWorld->dpvsDyn.dynEntClientCount[0]);
        gfxWorld->sceneDynBrush = NULL;
        memset(gfxWorld->sceneDynModel, 0, sizeof(GfxSceneDynModel) * gfxWorld->dpvsDyn.dynEntClientCount[0]);
    }

    bool GfxWorldLinker::updateOutdoors(GfxWorld* gfxWorld)
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
            con::error("ERROR! unable to find outdoor image $outdoor!");
            return false;
        }
        gfxWorld->outdoorImage = outdoorImageAsset->Asset();

        return true;
    }

    AssetCreationResult GfxWorldLinker::linkGfxWorld(BSPData* bsp)
    {
        GfxWorld* gfxWorld = m_memory.Alloc<GfxWorld>();
        gfxWorld->baseName = m_memory.Dup(bsp->name.c_str());
        gfxWorld->name = m_memory.Dup(bsp->bspName.c_str());

        // Default values taken from official maps
        gfxWorld->lightingFlags = 0;
        gfxWorld->lightingQuality = 4096;

        cleanGfxWorld(gfxWorld);

        if (!overwriteMapSurfaces(bsp, gfxWorld))
            return AssetCreationResult::Failure();

        overwriteMapSModels(bsp, gfxWorld);

        if (!overwriteLightmapData(gfxWorld))
            return AssetCreationResult::Failure();

        overwriteSkyBox(bsp, gfxWorld);

        if (!updateReflectionProbeData(gfxWorld))
            return AssetCreationResult::Failure();

        // world bounds are based on surface mins/maxs
        // Other update functions depend on the bounds being set first
        updateWorldBounds(gfxWorld);

        if (!updateOutdoors(gfxWorld))
            return AssetCreationResult::Failure();

        // gfx cells depend on surface/smodel count
        updateGfxCells(gfxWorld);

        overwriteLightGrid(gfxWorld);

        overwriteGfxLights(gfxWorld);

        overwriteModels(gfxWorld);

        updateSunData(gfxWorld);

        updateDynEntData(gfxWorld);

        auto gfxWorldAsset = m_context.AddAsset<AssetGfxWorld>(gfxWorld->name, gfxWorld);
        return AssetCreationResult::Success(gfxWorldAsset);
    }
}

