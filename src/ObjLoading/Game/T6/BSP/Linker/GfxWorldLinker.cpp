#include "GfxWorldLinker.h"

#include "../BSPCalculation.h"
#include "../BSPUtil.h"
#include "Utils/Pack.h"

namespace
{
    unsigned int packLmapCoord(float X, float Y)
    {
        float XStep = X * 65535.0f;
        float YStep = Y * 65535.0f;

        unsigned int result = 0;
        result |= (unsigned int)XStep;
        result &= 0xFFFF;
        result |= (unsigned int)YStep << 16;
        result &= 0xFFFFFFFF;

        return result;
    }

    constexpr const char* DEFAULT_IMAGE_NAME = ",mc/lambert1";
    constexpr char DEFAULT_PRIMARYLIGHT_INDEX = 1; // max 254
    constexpr char DEFAULT_LIGHTMAP_INDEX = 0;     // max 30
    constexpr char DEFAULT_RPROBE_INDEX = 0;       // max 254
    constexpr float XMODEL_CULL_DIST = 10000.0f;
    constexpr char DEFAULT_LIGHTGRID_COLOUR = 32;
} // namespace

namespace BSP
{
    GfxWorldLinker::GfxWorldLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
          m_search_path(searchPath),
          m_context(context)
    {
    }

    void GfxWorldLinker::loadDrawData(BSPData* bsp, GfxWorld* gfxWorld)
    {
        size_t vertexCount = bsp->gfxWorld.vertices.size();
        gfxWorld->draw.vertexCount = static_cast<unsigned int>(vertexCount);
        gfxWorld->draw.vertexDataSize0 = static_cast<unsigned int>(vertexCount * sizeof(GfxPackedWorldVertex));
        GfxPackedWorldVertex* vertexBuffer = m_memory.Alloc<GfxPackedWorldVertex>(vertexCount);
        for (size_t vertIdx = 0; vertIdx < vertexCount; vertIdx++)
        {
            BSPVertex& bspVertex = bsp->gfxWorld.vertices.at(vertIdx);
            GfxPackedWorldVertex* gfxVertex = &vertexBuffer[vertIdx];

            gfxVertex->xyz = bspVertex.pos;
            gfxVertex->color.packed = pack32::Vec4PackGfxColor(bspVertex.color.v);
            gfxVertex->texCoord.packed = pack32::Vec2PackTexCoordsUV(bspVertex.texCoord.v);
            gfxVertex->normal.packed = pack32::Vec3PackUnitVecThirdBased(bspVertex.normal.v);
            gfxVertex->tangent.packed = pack32::Vec3PackUnitVecThirdBased(bspVertex.tangent.v);
            gfxVertex->binormalSign = bspVertex.binormal.v[0] > 0.0f ? 1.0f : -1.0f;
            gfxVertex->lmapCoord.packed = packLmapCoord(0.0f, 0.0f);
        }
        gfxWorld->draw.vd0.data = reinterpret_cast<char*>(vertexBuffer);

        // vd1 is unused but still needs to be initialised
        // the data type varies and 0x20 is enough for all types
        gfxWorld->draw.vertexDataSize1 = 0x20;
        gfxWorld->draw.vd1.data = m_memory.Alloc<char>(gfxWorld->draw.vertexDataSize1);

        size_t indexCount = bsp->gfxWorld.indices.size();
        assert(indexCount % 3 == 0);
        gfxWorld->draw.indexCount = static_cast<int>(indexCount);
        gfxWorld->draw.indices = m_memory.Alloc<uint16_t>(indexCount); // overflow checked in bspcreator
        static_assert(sizeof(bsp->gfxWorld.indices.data()[0]) == sizeof(uint16_t));
        memcpy(gfxWorld->draw.indices, bsp->gfxWorld.indices.data(), sizeof(uint16_t) * indexCount);
    }

    bool GfxWorldLinker::loadMapSurfaces(BSPData* bsp, GfxWorld* gfxWorld)
    {
        loadDrawData(bsp, gfxWorld);

        size_t staticSurfaceCount = bsp->gfxWorld.staticSurfaces.size();
        size_t scriptSurfaceCount = bsp->gfxWorld.scriptSurfaces.size();
        size_t totalSurfaceCount = staticSurfaceCount + scriptSurfaceCount;

        if (staticSurfaceCount > 0xffff)
        {
            con::error("There are more than 65535 Static surfaces. count: {}", staticSurfaceCount);
            return false;
        }

        // Static surfaces go first in the array then script surfaces after
        gfxWorld->surfaceCount = static_cast<int>(totalSurfaceCount);
        gfxWorld->dpvs.staticSurfaceCount = static_cast<unsigned int>(staticSurfaceCount);
        gfxWorld->dpvs.surfaces = m_memory.Alloc<GfxSurface>(totalSurfaceCount);
        for (size_t surfIdx = 0; surfIdx < totalSurfaceCount; surfIdx++)
        {
            BSPSurface bspSurface;
            if (surfIdx < staticSurfaceCount)
                bspSurface = bsp->gfxWorld.staticSurfaces.at(surfIdx);
            else
                bspSurface = bsp->gfxWorld.scriptSurfaces.at(surfIdx - staticSurfaceCount);
            GfxSurface* gfxSurface = &gfxWorld->dpvs.surfaces[surfIdx];

            gfxSurface->tris.triCount = bspSurface.triCount; // overflow checked in bspcreator
            gfxSurface->tris.baseIndex = bspSurface.indexOfFirstIndex;
            gfxSurface->tris.vertexCount = bspSurface.vertexCount; // overflow checked in bspcreator
            gfxSurface->tris.firstVertex = bspSurface.indexOfFirstVertex;

            gfxSurface->tris.vertexDataOffset0 = bspSurface.indexOfFirstVertex * sizeof(GfxPackedWorldVertex);
            gfxSurface->tris.vertexDataOffset1 = 0; // vd1 is unused

            BSPMaterial bspMaterial = bsp->gfxWorld.materials.at(bspSurface.materialIndex);

            std::string materialName;
            if (bspMaterial.materialType == MATERIAL_TYPE_TEXTURE)
                materialName = bspMaterial.materialName;
            else // MATERIAL_TYPE_COLOUR
                materialName = DEFAULT_IMAGE_NAME;

            auto surfMaterialAsset = m_context.LoadDependency<AssetMaterial>(materialName);
            if (surfMaterialAsset == nullptr)
            {
                surfMaterialAsset = m_context.LoadDependency<AssetMaterial>(DEFAULT_IMAGE_NAME);
                assert(surfMaterialAsset != nullptr);
            }
            gfxSurface->material = surfMaterialAsset->Asset();

            GfxPackedWorldVertex* firstVert = reinterpret_cast<GfxPackedWorldVertex*>(&gfxWorld->draw.vd0.data[gfxSurface->tris.vertexDataOffset0]);
            gfxSurface->bounds[0] = firstVert[0].xyz;
            gfxSurface->bounds[1] = firstVert[0].xyz;
            for (size_t indexIdx = 0; indexIdx < static_cast<size_t>(gfxSurface->tris.triCount * 3); indexIdx++)
            {
                uint16_t vertIndex = gfxWorld->draw.indices[gfxSurface->tris.baseIndex + indexIdx];
                BSPUtil::updateAABBWithPoint(firstVert[vertIndex].xyz, gfxSurface->bounds[0], gfxSurface->bounds[1]);
            }
            gfxSurface->tris.mins = gfxSurface->bounds[0];
            gfxSurface->tris.maxs = gfxSurface->bounds[1];

            gfxSurface->flags = 0;
            if ((bspMaterial.contentFlags & BSPFlags::surfaceTypeToFlagMap[BSPFlags::SURF_TYPE_SKY].contentFlags) != 0)
                gfxSurface->flags |= GFX_SURFACE_IS_SKY;
            if ((bspMaterial.contentFlags & BSPFlags::surfaceTypeToFlagMap[BSPFlags::SURF_TYPE_NODRAW].contentFlags) != 0)
                gfxSurface->flags |= GFX_SURFACE_NO_DRAW;
            if ((bspMaterial.surfaceFlags & BSPFlags::surfaceTypeToFlagMap[BSPFlags::SURF_TYPE_ONLYCASTSHADOW].surfaceFlags) != 0)
                gfxSurface->flags |= GFX_SURFACE_CASTS_SUN_SHADOW;
            if ((bspMaterial.surfaceFlags & BSPFlags::surfaceTypeToFlagMap[BSPFlags::SURF_TYPE_NOCASTSHADOW].surfaceFlags) == 0)
                gfxSurface->flags |= GFX_SURFACE_CASTS_SHADOW;

            gfxSurface->primaryLightIndex = DEFAULT_PRIMARYLIGHT_INDEX;
            gfxSurface->lightmapIndex = DEFAULT_LIGHTMAP_INDEX;
            gfxSurface->reflectionProbeIndex = DEFAULT_RPROBE_INDEX;
        }

        // Some code uses Sorted surfs to index surfaces, so for simplicity keep the indexes sequential and from 0
        gfxWorld->dpvs.sortedSurfIndex = m_memory.Alloc<uint16_t>(staticSurfaceCount);
        for (size_t surfIdx = 0; surfIdx < staticSurfaceCount; surfIdx++)
            gfxWorld->dpvs.sortedSurfIndex[surfIdx] = static_cast<uint16_t>(surfIdx);

        // surface materials are written to by the game
        gfxWorld->dpvs.surfaceMaterials = m_memory.Alloc<GfxDrawSurf_align4>(staticSurfaceCount);

        // set all surface types to lit opaque
        gfxWorld->dpvs.litSurfsBegin = 0;
        gfxWorld->dpvs.litSurfsEnd = static_cast<unsigned int>(staticSurfaceCount);
        gfxWorld->dpvs.emissiveOpaqueSurfsBegin = static_cast<unsigned int>(staticSurfaceCount);
        gfxWorld->dpvs.emissiveOpaqueSurfsEnd = static_cast<unsigned int>(staticSurfaceCount);
        gfxWorld->dpvs.emissiveTransSurfsBegin = static_cast<unsigned int>(staticSurfaceCount);
        gfxWorld->dpvs.emissiveTransSurfsEnd = static_cast<unsigned int>(staticSurfaceCount);
        gfxWorld->dpvs.litTransSurfsBegin = static_cast<unsigned int>(staticSurfaceCount);
        gfxWorld->dpvs.litTransSurfsEnd = static_cast<unsigned int>(staticSurfaceCount);

        // visdata is written to by the game
        // all visdata is alligned by 128
        size_t allignedSurfaceCount = BSPUtil::allignBy128(staticSurfaceCount);
        gfxWorld->dpvs.surfaceVisDataCount = static_cast<unsigned int>(allignedSurfaceCount);
        gfxWorld->dpvs.surfaceVisData[0] = m_memory.Alloc<char>(allignedSurfaceCount);
        gfxWorld->dpvs.surfaceVisData[1] = m_memory.Alloc<char>(allignedSurfaceCount);
        gfxWorld->dpvs.surfaceVisData[2] = m_memory.Alloc<char>(allignedSurfaceCount);
        gfxWorld->dpvs.surfaceVisDataCameraSaved = m_memory.Alloc<char>(allignedSurfaceCount);
        gfxWorld->dpvs.surfaceCastsShadow = m_memory.Alloc<char>(allignedSurfaceCount);
        gfxWorld->dpvs.surfaceCastsSunShadow = m_memory.Alloc<char>(allignedSurfaceCount);

        return true;
    }

    bool GfxWorldLinker::loadXModels(BSPData* bsp, GfxWorld* gfxWorld)
    {
        size_t modelCount = bsp->gfxWorld.xmodels.size();
        gfxWorld->dpvs.smodelCount = modelCount;
        gfxWorld->dpvs.smodelInsts = m_memory.Alloc<GfxStaticModelInst>(modelCount);
        gfxWorld->dpvs.smodelDrawInsts = m_memory.Alloc<GfxStaticModelDrawInst>(modelCount);

        if (modelCount == 0)
            return true;
        if (modelCount > 0xFFFF)
        {
            con::error("GFXWorld exceeded the maximum number of static xmodels (65535 max, map count: {})", modelCount);
            return false;
        }

        char color = DEFAULT_LIGHTGRID_COLOUR;
        float sunIntensity = bsp->sunlight.intensity;
        if (sunIntensity < 0.0f || sunIntensity > 20000.0f)
        {
            con::error("Sun intensity ({}) needs to be between 0 and 20000", sunIntensity);
            return false;
        }
        else
        {
            float normalised = sunIntensity / 20000.0f;
            normalised *= 255.0f;
            float clean = roundf(normalised);
            color = static_cast<char>(clean);
        }

        for (size_t modelIdx = 0; modelIdx < modelCount; modelIdx++)
        {
            auto currModel = &gfxWorld->dpvs.smodelDrawInsts[modelIdx];
            auto currModelInst = &gfxWorld->dpvs.smodelInsts[modelIdx];
            BSPXModel& bspModel = bsp->gfxWorld.xmodels.at(modelIdx);

            auto xModelAsset = m_context.LoadDependency<AssetXModel>(bspModel.name);
            if (xModelAsset == nullptr)
            {
                con::error("Unable to load xmodel asset: \"{}\"", bspModel.name);
                return false;
            }
            else
                currModel->model = (XModel*)xModelAsset->Asset();

            currModel->placement.origin = bspModel.origin;
            BSPUtil::convertQuaternionToAxis(&bspModel.rotationQuaternion, currModel->placement.axis);
            currModel->placement.scale = bspModel.scale;

            currModel->flags = 0;
            if (!bspModel.doesCastShadow)
                currModel->flags |= STATIC_MODEL_FLAG_NO_SHADOW;

            currModel->cullDist = XMODEL_CULL_DIST;
            currModel->primaryLightIndex = DEFAULT_PRIMARYLIGHT_INDEX;
            currModel->reflectionProbeIndex = DEFAULT_RPROBE_INDEX;
            currModel->smid = modelIdx;

            currModelInst->lightingOrigin = bspModel.origin;

            if (!xModelAsset->IsReference())
            {
                BSPUtil::calculateXmodelBounds(currModel->model, currModel->placement.axis, currModelInst->mins, currModelInst->maxs);
                currModelInst->mins.x = (currModelInst->mins.x * bspModel.scale) + bspModel.origin.x;
                currModelInst->mins.y = (currModelInst->mins.y * bspModel.scale) + bspModel.origin.y;
                currModelInst->mins.z = (currModelInst->mins.z * bspModel.scale) + bspModel.origin.z;
                currModelInst->maxs.x = (currModelInst->maxs.x * bspModel.scale) + bspModel.origin.x;
                currModelInst->maxs.y = (currModelInst->maxs.y * bspModel.scale) + bspModel.origin.y;
                currModelInst->maxs.z = (currModelInst->maxs.z * bspModel.scale) + bspModel.origin.z;

                for (uint16_t lodIdx = 0; lodIdx < currModel->model->numLods; lodIdx++)
                {
                    uint16_t vertCount = 0;
                    for (auto surfaceIndex = 0u; surfaceIndex < currModel->model->lodInfo[lodIdx].numsurfs; surfaceIndex++)
                    {
                        const auto& surface = currModel->model->surfs[surfaceIndex + currModel->model->lodInfo[lodIdx].surfIndex];
                        vertCount += surface.vertCount;
                    }

                    currModel->lmapVertexInfo[lodIdx].numLmapVertexColors = vertCount;
                    currModel->lmapVertexInfo[lodIdx].lmapVertexColors = m_memory.Alloc<unsigned int>(vertCount);
                    memset(currModel->lmapVertexInfo[lodIdx].lmapVertexColors, color, sizeof(unsigned int) * vertCount);
                }
            }
            else
            {
                if (bspModel.areBoundsValid)
                {
                    currModelInst->mins = bspModel.mins;
                    currModelInst->maxs = bspModel.maxs;
                }
                else
                {
                    con::warn("Unable to determine the bounds of xmodel: \"{}\"", bspModel.name);
                    currModelInst->mins.x = bspModel.origin.x - 1.0f;
                    currModelInst->mins.y = bspModel.origin.y - 1.0f;
                    currModelInst->mins.z = bspModel.origin.z - 1.0f;
                    currModelInst->maxs.x = bspModel.origin.x + 1.0f;
                    currModelInst->maxs.y = bspModel.origin.y + 1.0f;
                    currModelInst->maxs.z = bspModel.origin.z + 1.0f;
                }
            }
        }

        // visdata is written to by the game
        // all visdata is alligned by 128
        size_t allignedModelCount = BSPUtil::allignBy128(modelCount);
        gfxWorld->dpvs.smodelVisDataCount = static_cast<unsigned int>(allignedModelCount);
        gfxWorld->dpvs.smodelVisData[0] = m_memory.Alloc<char>(allignedModelCount);
        gfxWorld->dpvs.smodelVisData[1] = m_memory.Alloc<char>(allignedModelCount);
        gfxWorld->dpvs.smodelVisData[2] = m_memory.Alloc<char>(allignedModelCount);
        gfxWorld->dpvs.smodelVisDataCameraSaved = m_memory.Alloc<char>(allignedModelCount);
        gfxWorld->dpvs.smodelCastsShadow = m_memory.Alloc<char>(allignedModelCount);
        for (size_t i = 0; i < modelCount; i++)
        {
            if ((gfxWorld->dpvs.smodelDrawInsts[i].flags & STATIC_MODEL_FLAG_NO_SHADOW) == 0)
                gfxWorld->dpvs.smodelCastsShadow[i] = 1;
        }

        return true;
    }

    void GfxWorldLinker::cleanGfxWorld(GfxWorld* gfxWorld)
    {
        // checksum is generated by the game
        gfxWorld->checksum = 0;

        // official maps set this to 0
        gfxWorld->dpvs.usageCount = 0;

        // Remove Coronas
        gfxWorld->coronaCount = 0;
        gfxWorld->coronas = nullptr;

        // Remove exposure volumes
        gfxWorld->exposureVolumeCount = 0;
        gfxWorld->exposureVolumes = nullptr;
        gfxWorld->exposureVolumePlaneCount = 0;
        gfxWorld->exposureVolumePlanes = nullptr;

        // Remove hero lights
        gfxWorld->heroLightCount = 0;
        gfxWorld->heroLights = nullptr;
        gfxWorld->heroLightTreeCount = 0;
        gfxWorld->heroLightTree = nullptr;

        // remove LUT data
        gfxWorld->lutVolumeCount = 0;
        gfxWorld->lutVolumes = nullptr;
        gfxWorld->lutVolumePlaneCount = 0;
        gfxWorld->lutVolumePlanes = nullptr;

        // remove occluders
        gfxWorld->numOccluders = 0;
        gfxWorld->occluders = nullptr;

        // remove Siege Skins
        gfxWorld->numSiegeSkinInsts = 0;
        gfxWorld->siegeSkinInsts = nullptr;

        // remove outdoor bounds
        gfxWorld->numOutdoorBounds = 0;
        gfxWorld->outdoorBounds = nullptr;

        // remove materials
        gfxWorld->ropeMaterial = nullptr;
        gfxWorld->lutMaterial = nullptr;
        gfxWorld->waterMaterial = nullptr;
        gfxWorld->coronaMaterial = nullptr;

        // remove shadow maps
        gfxWorld->shadowMapVolumeCount = 0;
        gfxWorld->shadowMapVolumes = nullptr;
        gfxWorld->shadowMapVolumePlaneCount = 0;
        gfxWorld->shadowMapVolumePlanes = nullptr;

        // remove stream info
        gfxWorld->streamInfo.aabbTreeCount = 0;
        gfxWorld->streamInfo.aabbTrees = nullptr;
        gfxWorld->streamInfo.leafRefCount = 0;
        gfxWorld->streamInfo.leafRefs = nullptr;

        // remove sun data
        memset(&gfxWorld->sun, 0, sizeof(sunflare_t));
        gfxWorld->sun.hasValidData = false;

        // Remove Water
        gfxWorld->waterDirection = 0.0f;
        gfxWorld->waterBuffers[0].bufferSize = 0;
        gfxWorld->waterBuffers[0].buffer = nullptr;
        gfxWorld->waterBuffers[1].bufferSize = 0;
        gfxWorld->waterBuffers[1].buffer = nullptr;

        // Remove Fog
        gfxWorld->worldFogModifierVolumeCount = 0;
        gfxWorld->worldFogModifierVolumes = nullptr;
        gfxWorld->worldFogModifierVolumePlaneCount = 0;
        gfxWorld->worldFogModifierVolumePlanes = nullptr;
        gfxWorld->worldFogVolumeCount = 0;
        gfxWorld->worldFogVolumes = nullptr;
        gfxWorld->worldFogVolumePlaneCount = 0;
        gfxWorld->worldFogVolumePlanes = nullptr;

        // materialMemory is unused
        gfxWorld->materialMemoryCount = 0;
        gfxWorld->materialMemory = nullptr;

        // sunLight is overwritten by the game, just needs to be a valid pointer
        gfxWorld->sunLight = m_memory.Alloc<GfxLight>();
    }

    bool GfxWorldLinker::loadGfxLights(BSPData* bsp, GfxWorld* gfxWorld)
    {
        // there must be 2 or more lights, first is the static light and second is the sun light
        gfxWorld->primaryLightCount = BSP_DEFAULT_LIGHT_COUNT + static_cast<unsigned int>(bsp->lights.size());
        gfxWorld->sunPrimaryLightIndex = SUN_LIGHT_INDEX;

        if (gfxWorld->primaryLightCount > 254)
        {
            con::error("Exceeded 254 lights in BSP, count: {}", gfxWorld->primaryLightCount);
            return false;
        }

        gfxWorld->shadowGeom = m_memory.Alloc<GfxShadowGeometry>(gfxWorld->primaryLightCount);
        for (unsigned int lightIdx = 0; lightIdx < gfxWorld->primaryLightCount; lightIdx++)
        {
            // smodelCount and smodelIndex is filled next loop
            gfxWorld->shadowGeom[lightIdx].smodelCount = 0;
            gfxWorld->shadowGeom[lightIdx].smodelIndex = m_memory.Alloc<uint16_t>(gfxWorld->dpvs.smodelCount);

            // sorted surfs and surfaceCount is recalculated each frame
            gfxWorld->shadowGeom[lightIdx].surfaceCount = gfxWorld->dpvs.staticSurfaceCount;
            gfxWorld->shadowGeom[lightIdx].sortedSurfIndex = m_memory.Alloc<uint16_t>(gfxWorld->dpvs.staticSurfaceCount);
        }
        for (unsigned int modelIdx = 0; modelIdx < gfxWorld->dpvs.smodelCount; modelIdx++)
        {
            if ((gfxWorld->dpvs.smodelDrawInsts[modelIdx].flags & STATIC_MODEL_FLAG_NO_SHADOW) != 0)
                continue;
            unsigned char lightIndex = gfxWorld->dpvs.smodelDrawInsts[modelIdx].primaryLightIndex;
            gfxWorld->shadowGeom[lightIndex].smodelIndex[gfxWorld->shadowGeom[lightIndex].smodelCount] = modelIdx;
            gfxWorld->shadowGeom[lightIndex].smodelCount++;
        }

        gfxWorld->lightRegion = m_memory.Alloc<GfxLightRegion>(gfxWorld->primaryLightCount);
        for (unsigned int lightIdx = 0; lightIdx < gfxWorld->primaryLightCount; lightIdx++)
        {
            gfxWorld->lightRegion[lightIdx].hullCount = 0;
            gfxWorld->lightRegion[lightIdx].hulls = nullptr;
        }

        unsigned int lightEntShadowVisSize = (gfxWorld->primaryLightCount - gfxWorld->sunPrimaryLightIndex - 1) * 8192;
        if (lightEntShadowVisSize != 0)
            gfxWorld->primaryLightEntityShadowVis = m_memory.Alloc<unsigned int>(lightEntShadowVisSize);
        else
            gfxWorld->primaryLightEntityShadowVis = nullptr;

        return true;
    }

    bool GfxWorldLinker::loadLightGrid(BSPData* bsp, GfxWorld* gfxWorld)
    {
        // world to lightgrid coords conversion:
        // l_x = (w_x + 131072.0f) / 32.0f;
        // l_y = (w_y + 131072.0f) / 32.0f;
        // l_z = (w_z + 131072.0f) / 64.0f;
        //
        // lightgrid to world coords conversion:
        // w_x = (double)l_x * 32.0f - 131072.0f;
        // w_y = (double)l_y * 32.0f - 131072.0f;
        // w_z = (double)l_z * 64.0f - 131072.0f;
        //
        // Lightrid coords are bounded by the uint16 limits:
        // w_minX = -131072, w_maxX = 1966048
        // w_minY = -131072, w_maxY = 1966048
        // w_minZ = -131072, w_maxZ = 4063168

        if (gfxWorld->mins.x < -131072.0f || gfxWorld->mins.y < -131072.0f || gfxWorld->mins.z < -131072.0f || gfxWorld->maxs.x > 1966048.0f
            || gfxWorld->maxs.y > 1966048.0f || gfxWorld->maxs.z > 4063168.0f)
        {
            con::error("Lightrid mins/maxs exceeded");
            return false;
        }

        // integer-only method used by the game
        gfxWorld->lightGrid.mins[0] = static_cast<uint16_t>((static_cast<int>(gfxWorld->mins.x) + 0x20000) >> 5);
        gfxWorld->lightGrid.mins[1] = static_cast<uint16_t>((static_cast<int>(gfxWorld->mins.y) + 0x20000) >> 5);
        gfxWorld->lightGrid.mins[2] = static_cast<uint16_t>((static_cast<int>(gfxWorld->mins.z) + 0x20000) >> 6);
        gfxWorld->lightGrid.maxs[0] = static_cast<uint16_t>((static_cast<int>(gfxWorld->maxs.x) + 0x20000) >> 5);
        gfxWorld->lightGrid.maxs[1] = static_cast<uint16_t>((static_cast<int>(gfxWorld->maxs.y) + 0x20000) >> 5);
        gfxWorld->lightGrid.maxs[2] = static_cast<uint16_t>((static_cast<int>(gfxWorld->maxs.z) + 0x20000) >> 6);

        gfxWorld->lightGrid.rowAxis = 0; // default value
        gfxWorld->lightGrid.colAxis = 1; // default value
        gfxWorld->lightGrid.sunPrimaryLightIndex = SUN_LIGHT_INDEX;
        gfxWorld->lightGrid.offset = 0.0f; // default value

        // setting all rowDataStart indexes to 0 will always index the first row in rawRowData
        int rowDataStartSize = gfxWorld->lightGrid.maxs[gfxWorld->lightGrid.rowAxis] - gfxWorld->lightGrid.mins[gfxWorld->lightGrid.rowAxis] + 1;
        gfxWorld->lightGrid.rowDataStart = m_memory.Alloc<uint16_t>(rowDataStartSize);

        // Adding 0x1FF so the lookup table will be 0x200 bytes in size
        gfxWorld->lightGrid.rawRowDataSize = static_cast<unsigned int>(sizeof(GfxLightGridRow) + 0x1FF);
        GfxLightGridRow* row = static_cast<GfxLightGridRow*>(m_memory.AllocRaw(gfxWorld->lightGrid.rawRowDataSize));
        row->colStart = 0;
        row->colCount = 0xFFFF;
        row->zStart = 0;
        row->zCount = 0xFF; // 0xFF as this seems to be the sweet spot of values
        row->firstEntry = 0;
        for (int i = 0; i < 0x200; i++)
            row->lookupTable[i] = -1; // values in the lookup table are subtracted from the sample pos until it reaches 0, so use 0xFF
        gfxWorld->lightGrid.rawRowData = reinterpret_cast<aligned_byte_pointer*>(row);

        // entries are looked up based on the lightgrid sample pos (given ingame) and the lightgrid lookup table
        gfxWorld->lightGrid.entryCount = 60000; // 60000 as it should be enough entries to be indexed by all lightgrid sample positions
        GfxLightGridEntry* entryArray = m_memory.Alloc<GfxLightGridEntry>(gfxWorld->lightGrid.entryCount);
        for (unsigned int i = 0; i < gfxWorld->lightGrid.entryCount; i++)
        {
            entryArray[i].colorsIndex = 0; // always index first colour
            entryArray[i].primaryLightIndex = DEFAULT_PRIMARYLIGHT_INDEX;
            entryArray[i].visibility = 0;
        }
        gfxWorld->lightGrid.entries = entryArray;

        char color = DEFAULT_LIGHTGRID_COLOUR;
        float sunIntensity = bsp->sunlight.intensity;
        if (sunIntensity < 0.0f || sunIntensity > 20000.0f)
        {
            con::error("Sun intensity ({}) needs to be between 0 and 20000", sunIntensity);
            return false;
        }
        else
        {
            float normalised = sunIntensity / 20000.0f;
            normalised *= 255.0f;
            float clean = roundf(normalised);
            color = static_cast<char>(clean);
        }

        // colours are looked up with a lightgrid entries colorsIndex
        gfxWorld->lightGrid.colorCount = 0x1000; // 0x1000 as it should be enough to hold every index
        gfxWorld->lightGrid.colors = m_memory.Alloc<GfxCompressedLightGridColors>(gfxWorld->lightGrid.colorCount);
        memset(gfxWorld->lightGrid.colors, color, rowDataStartSize * sizeof(uint16_t));

        // we use the colours array instead of coeffs array
        gfxWorld->lightGrid.coeffCount = 0;
        gfxWorld->lightGrid.coeffs = nullptr;
        gfxWorld->lightGrid.skyGridVolumeCount = 0;
        gfxWorld->lightGrid.skyGridVolumes = nullptr;

        return true;
    }

    struct mnode_t
    {
        unsigned __int16 cellIndex;
        unsigned __int16 rightChildOffset;
    };

    void GfxWorldLinker::loadGfxCells(GfxWorld* gfxWorld)
    {
        // Cells are basically data used to determine what can be seen and what cant be seen
        // Right now custom maps have no optimisation so there is only 1 cell
        int cellCount = 1;

        gfxWorld->dpvsPlanes.cellCount = cellCount;
        gfxWorld->cellBitsCount = ((cellCount + 127) >> 3) & 0x1FFFFFF0;

        int cellCasterBitsCount = cellCount * ((cellCount + 31) / 32);
        gfxWorld->cellCasterBits = m_memory.Alloc<unsigned int>(cellCasterBitsCount);

        int sceneEntCellBitsCount = cellCount * 512;
        gfxWorld->dpvsPlanes.sceneEntCellBits = m_memory.Alloc<unsigned int>(sceneEntCellBitsCount);

        gfxWorld->cells = m_memory.Alloc<GfxCell>(cellCount);
        gfxWorld->cells[0].portalCount = 0;
        gfxWorld->cells[0].portals = nullptr;
        gfxWorld->cells[0].mins = gfxWorld->mins;
        gfxWorld->cells[0].maxs = gfxWorld->maxs;

        // there is only 1 reflection probe
        gfxWorld->cells[0].reflectionProbeCount = 1;
        gfxWorld->cells[0].reflectionProbes = m_memory.Alloc<char>(gfxWorld->cells[0].reflectionProbeCount);
        gfxWorld->cells[0].reflectionProbes[0] = DEFAULT_RPROBE_INDEX;

        // AABB trees are used to detect what should be rendered and what shouldn't
        // Just use the first AABB node to hold all models, no optimisation but all models/surfaces wil lbe drawn
        gfxWorld->cells[0].aabbTreeCount = 1;
        gfxWorld->cells[0].aabbTree = m_memory.Alloc<GfxAabbTree>(gfxWorld->cells[0].aabbTreeCount);
        gfxWorld->cells[0].aabbTree[0].childCount = 0;
        gfxWorld->cells[0].aabbTree[0].childrenOffset = 0;
        gfxWorld->cells[0].aabbTree[0].startSurfIndex = 0;
        gfxWorld->cells[0].aabbTree[0].surfaceCount = static_cast<uint16_t>(gfxWorld->dpvs.staticSurfaceCount);
        gfxWorld->cells[0].aabbTree[0].smodelIndexCount = static_cast<uint16_t>(gfxWorld->dpvs.smodelCount);
        gfxWorld->cells[0].aabbTree[0].smodelIndexes = m_memory.Alloc<unsigned short>(gfxWorld->dpvs.smodelCount);
        for (unsigned short smodelIdx = 0; smodelIdx < gfxWorld->dpvs.smodelCount; smodelIdx++)
        {
            gfxWorld->cells[0].aabbTree[0].smodelIndexes[smodelIdx] = smodelIdx;
        }
        gfxWorld->cells[0].aabbTree[0].mins = gfxWorld->mins;
        gfxWorld->cells[0].aabbTree[0].maxs = gfxWorld->maxs;

        // nodes have the struct mnode_t, and there must be at least 1 node (similar to BSP nodes)
        // Nodes mnode_t.cellIndex indexes gfxWorld->cells
        // and (mnode_t.cellIndex - (world->dpvsPlanes.cellCount + 1) indexes world->dpvsPlanes.planes
        // Use only one node as there is no optimisation in custom maps

        gfxWorld->nodeCount = 1;
        gfxWorld->dpvsPlanes.nodes = m_memory.Alloc<uint16_t>(gfxWorld->nodeCount);
        gfxWorld->dpvsPlanes.nodes[0] = 1; // nodes reference cells by index + 1

        // planes are overwritten by the clipmap loading code ingame
        gfxWorld->planeCount = 0;
        gfxWorld->dpvsPlanes.planes = nullptr;
    }

    void GfxWorldLinker::loadWorldBounds(GfxWorld* gfxWorld)
    {
        gfxWorld->mins = gfxWorld->dpvs.surfaces[0].bounds[0];
        gfxWorld->maxs = gfxWorld->dpvs.surfaces[0].bounds[1];
        for (int surfIdx = 0; surfIdx < gfxWorld->surfaceCount; surfIdx++)
        {
            BSPUtil::updateAABB(gfxWorld->dpvs.surfaces[surfIdx].bounds[0], gfxWorld->dpvs.surfaces[surfIdx].bounds[1], gfxWorld->mins, gfxWorld->maxs);
        }

        for (unsigned int smodeldx = 0; smodeldx < gfxWorld->dpvs.smodelCount; smodeldx++)
        {
            BSPUtil::updateAABB(gfxWorld->dpvs.smodelInsts[smodeldx].mins, gfxWorld->dpvs.smodelInsts[smodeldx].maxs, gfxWorld->mins, gfxWorld->maxs);
        }
    }

    void GfxWorldLinker::loadModels(BSPData* bsp, GfxWorld* gfxWorld)
    {
        // Models (Submodels in the clipmap code) are used for the world and map ent collision (triggers, bomb zones, etc)
        // bounds are checked in clipmap linker
        gfxWorld->modelCount = static_cast<int>(bsp->models.size() + 1);
        gfxWorld->models = m_memory.Alloc<GfxBrushModel>(gfxWorld->modelCount);

        // first model is always the world model
        gfxWorld->models[0].startSurfIndex = 0;
        gfxWorld->models[0].surfaceCount = static_cast<unsigned int>(gfxWorld->dpvs.staticSurfaceCount);
        for (unsigned int surfIdx = 0; surfIdx < gfxWorld->dpvs.staticSurfaceCount; surfIdx++)
        {
            if (surfIdx == 0)
            {
                gfxWorld->models[0].bounds[0] = gfxWorld->dpvs.surfaces[surfIdx].bounds[0];
                gfxWorld->models[0].bounds[1] = gfxWorld->dpvs.surfaces[surfIdx].bounds[1];
            }
            else
                BSPUtil::updateAABB(gfxWorld->dpvs.surfaces[surfIdx].bounds[0],
                                    gfxWorld->dpvs.surfaces[surfIdx].bounds[1],
                                    gfxWorld->models[0].bounds[0],
                                    gfxWorld->models[0].bounds[1]);
        }
        if (gfxWorld->dpvs.staticSurfaceCount == 0)
        {
            gfxWorld->models[0].bounds[0] = {};
            gfxWorld->models[0].bounds[1] = {};
        }
        memset(&gfxWorld->models[0].writable, 0, sizeof(GfxBrushModelWritable));

        for (size_t modelIdx = 0; modelIdx < bsp->models.size(); modelIdx++)
        {
            auto currEntModel = &gfxWorld->models[modelIdx + 1];
            auto& bspModel = bsp->models.at(modelIdx);

            if (bspModel.isGfxModel && bspModel.surfaceCount != 0)
            {
                currEntModel->startSurfIndex = bsp->gfxWorld.staticSurfaces.size() + bspModel.surfaceIndex;
                currEntModel->surfaceCount = bspModel.surfaceCount;
                for (size_t surfIdx = 0; surfIdx < bspModel.surfaceCount; surfIdx++)
                {
                    GfxSurface* surf = &gfxWorld->dpvs.surfaces[currEntModel->startSurfIndex + surfIdx];
                    if (surfIdx == 0)
                    {
                        currEntModel->bounds[0] = surf->bounds[0];
                        currEntModel->bounds[1] = surf->bounds[1];
                    }
                    else
                        BSPUtil::updateAABB(surf->bounds[0], surf->bounds[1], currEntModel->bounds[0], currEntModel->bounds[1]);
                }
            }
            else
            {
                currEntModel->startSurfIndex = -1; // -1 when it doesn't use map surfaces
                currEntModel->surfaceCount = 0;
                currEntModel->bounds[0] = {};
                currEntModel->bounds[1] = {};
            }
            memset(&gfxWorld->models[0].writable, 0, sizeof(GfxBrushModelWritable));
        }
    }

    void GfxWorldLinker::loadSunData(BSPData* bsp, GfxWorld* gfxWorld)
    {
        BSPLight& sunlight = bsp->sunlight;
        vec3_t viewAngles = BSPUtil::convertForwardVectorToViewAngles(sunlight.direction);
        gfxWorld->sunParse.initWorldSun->angles = viewAngles;
        gfxWorld->sunParse.initWorldSun->sunCd.x = sunlight.colour.x;
        gfxWorld->sunParse.initWorldSun->sunCd.y = sunlight.colour.y;
        gfxWorld->sunParse.initWorldSun->sunCd.z = sunlight.colour.z;
        gfxWorld->sunParse.initWorldSun->sunCd.w = 1.0f;

        // fog is not implemented yet, values taken from mp_dig
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

    bool GfxWorldLinker::loadReflectionProbeData(GfxWorld* gfxWorld)
    {
        gfxWorld->draw.reflectionProbeCount = 1;

        gfxWorld->draw.reflectionProbeTextures = m_memory.Alloc<GfxTexture>(gfxWorld->draw.reflectionProbeCount);

        // default values taken from mp_dig
        gfxWorld->draw.reflectionProbes = m_memory.Alloc<GfxReflectionProbe>(gfxWorld->draw.reflectionProbeCount);
        gfxWorld->draw.reflectionProbes[0].mipLodBias = -8.0; // always -8.0f
        gfxWorld->draw.reflectionProbes[0].origin = {};
        gfxWorld->draw.reflectionProbes[0].lightingSH.V0 = {};
        gfxWorld->draw.reflectionProbes[0].lightingSH.V1 = {};
        gfxWorld->draw.reflectionProbes[0].lightingSH.V2 = {};

        gfxWorld->draw.reflectionProbes[0].probeVolumeCount = 0;
        gfxWorld->draw.reflectionProbes[0].probeVolumes = nullptr;

        std::string probeImageName = ",$black";
        auto probeImageAsset = m_context.LoadDependency<AssetImage>(probeImageName);
        if (probeImageAsset == nullptr)
        {
            con::error("ERROR! unable to find reflection probe image {}!", probeImageName);
            return false;
        }
        gfxWorld->draw.reflectionProbes[0].reflectionImage = probeImageAsset->Asset();

        return true;
    }

    bool GfxWorldLinker::loadLightmapData(GfxWorld* gfxWorld)
    {
        gfxWorld->draw.lightmapCount = 1;

        gfxWorld->draw.lightmapPrimaryTextures = m_memory.Alloc<GfxTexture>(gfxWorld->draw.lightmapCount);
        gfxWorld->draw.lightmapSecondaryTextures = m_memory.Alloc<GfxTexture>(gfxWorld->draw.lightmapCount);

        std::string secondaryTexture = ",$gray"; //  gray makes shadows a nice looking shade of black
        auto secondaryTextureAsset = m_context.LoadDependency<AssetImage>(secondaryTexture);
        if (secondaryTextureAsset == nullptr)
        {
            con::error("ERROR! unable to find lightmap image {}!", secondaryTexture);
            return false;
        }
        gfxWorld->draw.lightmaps = m_memory.Alloc<GfxLightmapArray>(gfxWorld->draw.lightmapCount);
        gfxWorld->draw.lightmaps[0].primary = nullptr; // always nullptr
        gfxWorld->draw.lightmaps[0].secondary = secondaryTextureAsset->Asset();

        return true;
    }

    void GfxWorldLinker::loadSkyBox(BSPData* projInfo, GfxWorld* gfxWorld)
    {
        std::string skyBoxName = "skybox_" + projInfo->name;
        gfxWorld->skyBoxModel = m_memory.Dup(skyBoxName.c_str());

        if (m_context.LoadDependency<AssetXModel>(skyBoxName) == nullptr)
        {
            con::warn("WARN: Unable to load the skybox xmodel {}!", skyBoxName);
        }

        // always 0 and 1
        gfxWorld->skyDynIntensity.angle0 = 0.0f;
        gfxWorld->skyDynIntensity.angle1 = 0.0f;
        gfxWorld->skyDynIntensity.factor0 = 1.0f;
        gfxWorld->skyDynIntensity.factor1 = 1.0f;
    }

    void GfxWorldLinker::loadDynEntData(GfxWorld* gfxWorld)
    {
        int dynEntCount = 0;
        gfxWorld->dpvsDyn.dynEntClientCount[0] = dynEntCount + 256; // the game allocs 256 empty dynents, as they may be used ingame
        gfxWorld->dpvsDyn.dynEntClientCount[1] = 0;

        // +100: there is a crash that happens when ragdolls are created, and dynEntClientWordCount[0] is the issue.
        // Making the value much larger than required fixes it, but unsure what the root cause is
        gfxWorld->dpvsDyn.dynEntClientWordCount[0] = ((gfxWorld->dpvsDyn.dynEntClientCount[0] + 31) >> 5) + 100;
        gfxWorld->dpvsDyn.dynEntClientWordCount[1] = 0;
        gfxWorld->dpvsDyn.usageCount = 0;

        int dynEntCellBitsSize = gfxWorld->dpvsDyn.dynEntClientWordCount[0] * gfxWorld->dpvsPlanes.cellCount;
        gfxWorld->dpvsDyn.dynEntCellBits[0] = m_memory.Alloc<unsigned int>(dynEntCellBitsSize);
        gfxWorld->dpvsDyn.dynEntCellBits[1] = nullptr;

        int dynEntVisData0Size = gfxWorld->dpvsDyn.dynEntClientWordCount[0] * 32;
        gfxWorld->dpvsDyn.dynEntVisData[0][0] = m_memory.Alloc<char>(dynEntVisData0Size);
        gfxWorld->dpvsDyn.dynEntVisData[0][1] = m_memory.Alloc<char>(dynEntVisData0Size);
        gfxWorld->dpvsDyn.dynEntVisData[0][2] = m_memory.Alloc<char>(dynEntVisData0Size);
        gfxWorld->dpvsDyn.dynEntVisData[1][0] = nullptr;
        gfxWorld->dpvsDyn.dynEntVisData[1][1] = nullptr;
        gfxWorld->dpvsDyn.dynEntVisData[1][2] = nullptr;

        unsigned int dynEntShadowVisCount = gfxWorld->dpvsDyn.dynEntClientCount[0] * (gfxWorld->primaryLightCount - gfxWorld->sunPrimaryLightIndex - 1);
        gfxWorld->primaryLightDynEntShadowVis[0] = m_memory.Alloc<unsigned int>(dynEntShadowVisCount);
        gfxWorld->primaryLightDynEntShadowVis[1] = nullptr;

        gfxWorld->sceneDynModel = m_memory.Alloc<GfxSceneDynModel>(gfxWorld->dpvsDyn.dynEntClientCount[0]);
        gfxWorld->sceneDynBrush = nullptr;
    }

    bool GfxWorldLinker::loadOutdoors(GfxWorld* gfxWorld)
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

        std::string outdoorImageName = std::string(",$black");
        auto outdoorImageAsset = m_context.LoadDependency<AssetImage>(outdoorImageName);
        if (outdoorImageAsset == nullptr)
        {
            con::error("ERROR! unable to find outdoor image!");
            return false;
        }
        gfxWorld->outdoorImage = outdoorImageAsset->Asset();

        return true;
    }

    GfxWorld* GfxWorldLinker::linkGfxWorld(BSPData* bsp)
    {
        GfxWorld* gfxWorld = m_memory.Alloc<GfxWorld>();
        gfxWorld->baseName = m_memory.Dup(bsp->name.c_str());
        gfxWorld->name = m_memory.Dup(bsp->bspName.c_str());

        // Default values taken from origins
        gfxWorld->lightingFlags = 4;
        gfxWorld->lightingQuality = 10000;

        cleanGfxWorld(gfxWorld);

        if (!loadMapSurfaces(bsp, gfxWorld))
            return nullptr;

        if (!loadXModels(bsp, gfxWorld))
            return nullptr;

        if (!loadLightmapData(gfxWorld))
            return nullptr;

        loadSkyBox(bsp, gfxWorld);

        if (!loadReflectionProbeData(gfxWorld))
            return nullptr;

        // world bounds are based on loaded surface mins/maxs and xmodels
        loadWorldBounds(gfxWorld);

        if (!loadOutdoors(gfxWorld)) // requires world mins/maxs
            return nullptr;

        // gfx cells depend on surface/smodel count
        loadGfxCells(gfxWorld);

        loadLightGrid(bsp, gfxWorld); // requires world mins/maxs

        if (!loadGfxLights(bsp, gfxWorld)) // requires xmodels and surfaces
            return nullptr;

        loadModels(bsp, gfxWorld); // requires surfaces

        loadSunData(bsp, gfxWorld);

        loadDynEntData(gfxWorld); // requires cells and lights

        return gfxWorld;
    }
} // namespace BSP
