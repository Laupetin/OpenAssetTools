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

    struct BSPWorld
    {
        std::vector<BSPSurface> surfaces;
        std::vector<BSPVertex> vertices;
        std::vector<uint16_t> indices;
        std::vector<BSPMaterial> materials;
        std::vector<BSPXModel> xmodels;
    };

    enum BSPLightType
    {
        LIGHT_TYPE_DIRECTIONAL,
        LIGHT_TYPE_POINT,
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
        bool hasPosBeenSet;

        // angle is in radians. only used on spot/dir lights
        float innerConeAngle;
        float outerConeAngle;
    };

    enum BSPSpawnPointType
    {
        SPAWNPOINT_TYPE_DEFENDER,
        SPAWNPOINT_TYPE_ATTACKER,
        SPAWNPOINT_TYPE_ALL,
    };

    struct BSPSpawnPoint
    {
        vec3_t origin;
        vec3_t forward;
        BSPSpawnPointType type;
    };

    struct BSPPathNode
    {
        vec3_t origin;
    };

    struct BSPData
    {
        std::string name;
        std::string bspName;
        bool isZombiesMap;

        BSPWorld gfxWorld;
        BSPWorld colWorld;

        std::vector<BSPLight> lights;
        std::vector<BSPSpawnPoint> spawnpoints;
        std::vector<BSPPathNode> pathnodes;
    };

    // BSPGameConstants:
    // These values are hardcoded ingame and will break the map if they are changed
    namespace BSPGameConstants
    {
        constexpr unsigned int MAX_COLLISION_VERTS = UINT16_MAX;

        constexpr size_t MAX_AABB_TREE_CHILDREN = 128;

        enum BSPDefaultLights
        {
            EMPTY_LIGHT_INDEX = 0,
            SUN_LIGHT_INDEX = 1,
            BSP_DEFAULT_LIGHT_COUNT = 2
        };
    } // namespace BSPGameConstants

    // BSPLinkingConstants:
    // These values are BSP linking constants that are required for the link to be successful
    namespace BSPLinkingConstants
    {
        constexpr const char* MISSING_IMAGE_NAME = ",mc/lambert1";
        constexpr const char* COLOR_ONLY_IMAGE_NAME = ",mc/lambert1";
    } // namespace BSPLinkingConstants

    // BSPEditableConstants:
    // These values are BSP constants that can be edited and may not break the linker/game if changed
    namespace BSPEditableConstants
    {
        // Default xmodel values
        // Unused as there is no support for xmodels right now
        constexpr float DEFAULT_SMODEL_CULL_DIST = 10000.0f;
        constexpr int DEFAULT_SMODEL_FLAGS = STATIC_MODEL_FLAG_NO_SHADOW;
        constexpr int DEFAULT_SMODEL_LIGHT = 1;
        constexpr int DEFAULT_SMODEL_REFLECTION_PROBE = 0;

        // Default surface values
        constexpr int DEFAULT_SURFACE_LIGHT = 1;
        constexpr int DEFAULT_SURFACE_LIGHTMAP = 0;
        constexpr int DEFAULT_SURFACE_REFLECTION_PROBE = 0;
        constexpr int DEFAULT_SURFACE_FLAGS = 0;

        // material flags determine the features of a surface
        // unsure which flag type changes what right now
        // -1 results in: no running, water splashes all the time, low friction, slanted angles make you slide very fast
        // 1 results in: normal surface features, grenades work, seems normal
        constexpr int MATERIAL_SURFACE_FLAGS = 1;
        constexpr int MATERIAL_CONTENT_FLAGS = 1;

        // terrain/world flags: does not change the type of terrain or what features they have
        // from testing, as long at it isn't 0 things will work correctly
        constexpr int LEAF_TERRAIN_CONTENTS = 1;
        constexpr int WORLD_TERRAIN_CONTENTS = 1;

        // lightgrid (global) lighting colour
        // since lightgrids are not well understood, this colour is used for the R, G and B values right now
        constexpr unsigned char LIGHTGRID_COLOUR = 128;

        // Sunlight values
        constexpr vec4_t SUNLIGHT_COLOR = {0.75f, 0.75f, 0.75f, 1.0f};
        constexpr vec3_t SUNLIGHT_DIRECTION = {0.0f, 0.0f, 0.0f};
    }; // namespace BSPEditableConstants
} // namespace BSP
