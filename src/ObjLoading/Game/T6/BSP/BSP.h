#pragma once

#include "BSPFlags.h"
#include "Game/T6/T6.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <memory>
#include <string>
#include <vector>
using namespace T6;

namespace BSP
{
    enum BSPMaterialType
    {
        MATERIAL_TYPE_COLOUR,
        MATERIAL_TYPE_TEXTURE
    };

    struct BSPVertex
    {
        vec3_t pos;
        vec4_t color;
        vec2_t texCoord;
        vec3_t normal;
        vec3_t tangent;
        vec3_t binormal;
    };

    struct BSPMaterial
    {
        std::string materialName;
        BSPMaterialType materialType;
        vec4_t materialColour;

        int surfaceFlags;
        int contentFlags;
    };

    struct BSPSurface
    {
        size_t materialIndex;
        uint16_t vertexCount;
        uint16_t triCount;
        int indexOfFirstVertex;
        int indexOfFirstIndex;
    };

    struct BSPXModel
    {
        std::string name;

        vec3_t origin;
        vec4_t rotationQuaternion;
        float scale;

        bool areBoundsValid;
        vec3_t mins;
        vec3_t maxs;

        bool doesCastShadow;
    };

    struct BSPBoxBrush
    {
        size_t vertexIndex;
        size_t vertexCount;

        int surfaceFlags;
        int contentFlags;
    };

    struct BSPWorld
    {
        std::vector<BSPSurface> staticSurfaces;
        std::vector<BSPSurface> scriptSurfaces;

        std::vector<BSPVertex> vertices;
        std::vector<uint16_t> indices;
        std::vector<BSPMaterial> materials;
        std::vector<BSPXModel> xmodels;

        std::vector<BSPBoxBrush> scriptBoxBrushes;
        std::vector<vec3_t> boxBrushVerts;
    };

    enum BSPLightType
    {
        LIGHT_TYPE_DIRECTIONAL,
        LIGHT_TYPE_SPOT
    };

    struct BSPLight
    {
        BSPLightType type;
        vec3_t colour;
        float range;
        float intensity;

        vec3_t pos;
        vec3_t direction;

        // angle is in radians. only used on spot lights
        float innerConeAngle;
        float outerConeAngle;
    };

    struct BSPSpawnPoint
    {
        vec3_t origin;
        vec3_t forward;
        std::string spawnpointGroupName;
    };

    struct BSPZoneZM
    {
        vec3_t origin;
        std::string zoneName;
        std::string spawnerGroupName;
        std::string spawnpointGroupName;
        size_t modelIndex;
    };

    struct BSPZSpawnerZM
    {
        std::string spawnerGroupName;
        vec3_t origin;
        vec3_t forward;
    };

    struct BSPModel
    {
        bool isGfxModel;

        size_t surfaceIndex;
        size_t surfaceCount;

        bool hasBrush;
        size_t brushIndex;
    };

    struct BSPEntityEntry
    {
        std::string key;
        std::string value;
    };

    struct BSPEntity
    {
        vec3_t origin;
        vec4_t rotationQuaternion;
        size_t modelIndex;

        std::vector<BSPEntityEntry> entries;
    };

    struct BSPData
    {
        std::string name;
        std::string bspName;
        bool isZombiesMap;

        BSPWorld gfxWorld;
        BSPWorld colWorld;

        bool hasSunlightBeenSet;
        BSPLight sunlight;
        std::vector<BSPLight> lights;

        std::vector<BSPSpawnPoint> spawnpoints;
        std::vector<BSPZoneZM> zm_zones;
        std::vector<BSPZSpawnerZM> zm_spawners;

        bool containsWorldspawn;
        bool containsIntermssion;
        std::vector<BSPEntity> entities;
        BSPEntity worldspawn;

        std::vector<BSPModel> models;
    };

    enum BSPDefaultLights
    {
        EMPTY_LIGHT_INDEX = 0,
        SUN_LIGHT_INDEX = 1,
        BSP_DEFAULT_LIGHT_COUNT = 2
    };
} // namespace BSP
