#pragma once

#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/IW4.h"
#include "StateMap/StateMapLayout.h"

#include <type_traits>
#include <unordered_map>

namespace IW4
{
    inline const char* techniqueTypeNames[]{
        "depth prepass",
        "build floatz",
        "build shadowmap depth",
        "build shadowmap color",
        "unlit",
        "emissive",
        "emissive dfog",
        "emissive shadow",
        "emissive shadow dfog",
        "lit",
        "lit dfog",
        "lit sun",
        "lit sun dfog",
        "lit sun shadow",
        "lit sun shadow dfog",
        "lit spot",
        "lit spot dfog",
        "lit spot shadow",
        "lit spot shadow dfog",
        "lit omni",
        "lit omni dfog",
        "lit omni shadow",
        "lit omni shadow dfog",
        "lit instanced",
        "lit instanced dfog",
        "lit instanced sun",
        "lit instanced sun dfog",
        "lit instanced sun shadow",
        "lit instanced sun shadow dfog",
        "lit instanced spot",
        "lit instanced spot dfog",
        "lit instanced spot shadow",
        "lit instanced spot shadow dfog",
        "lit instanced omni",
        "lit instanced omni dfog",
        "lit instanced omni shadow",
        "lit instanced omni shadow dfog",
        "light spot",
        "light omni",
        "light spot shadow",
        "fakelight normal",
        "fakelight view",
        "sunlight preview",
        "case texture",
        "solid wireframe",
        "shaded wireframe",
        "debug bumpmap",
        "debug bumpmap instanced",
    };
    static_assert(std::extent_v<decltype(techniqueTypeNames)> == TECHNIQUE_COUNT);

    static const char* materialStreamDestinationNames[]{
        "position",
        "normal",
        "color[0]",
        "color[1]",
        "depth",
        "texcoord[0]",
        "texcoord[1]",
        "texcoord[2]",
        "texcoord[3]",
        "texcoord[4]",
        "texcoord[5]",
        "texcoord[6]",
        "texcoord[7]",
    };
    static_assert(std::extent_v<decltype(materialStreamDestinationNames)> == STREAM_DST_COUNT);
    static const char* materialStreamDestinationAbbreviation[]{
        "p",
        "n",
        "c0",
        "c1",
        "d",
        "t0",
        "t1",
        "t2",
        "t3",
        "t4",
        "t5",
        "t6",
        "t7",
    };
    static_assert(std::extent_v<decltype(materialStreamDestinationAbbreviation)> == STREAM_DST_COUNT);

    static const char* materialStreamSourceNames[]{
        "position",
        "color",
        "texcoord[0]",
        "normal",
        "tangent",
        "texcoord[1]",
        "texcoord[2]",
        "normalTransform[0]",
        "normalTransform[1]",
    };
    static_assert(std::extent_v<decltype(materialStreamSourceNames)> == STREAM_SRC_COUNT);
    static const char* materialStreamSourceAbbreviation[]{
        "p",
        "c",
        "t0",
        "n",
        "t",
        "t1",
        "t2",
        "n0",
        "n1",
    };
    static_assert(std::extent_v<decltype(materialStreamSourceAbbreviation)> == STREAM_SRC_COUNT);

    inline CodeSamplerSource s_lightmapSamplers[]{
        {"primary", TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY, nullptr, 0, 0},
        {"secondary", TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY, nullptr, 0, 0},
        {},
    };

    inline CodeSamplerSource s_lightSamplers[]{
        {"attenuation", TEXTURE_SRC_CODE_LIGHT_ATTENUATION, nullptr, 0, 0},
        {},
    };

    inline CodeSamplerSource s_codeSamplers[]{
        {"white", TEXTURE_SRC_CODE_WHITE, nullptr, 0, 0},
        {"black", TEXTURE_SRC_CODE_BLACK, nullptr, 0, 0},
        {"identityNormalMap", TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP, nullptr, 0, 0},
        {"lightmap", TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY, s_lightmapSamplers, 0, 0},
        {"outdoor", TEXTURE_SRC_CODE_OUTDOOR, nullptr, 0, 0},
        {"shadowmapSun", TEXTURE_SRC_CODE_SHADOWMAP_SUN, nullptr, 0, 0},
        {"shadowmapSpot", TEXTURE_SRC_CODE_SHADOWMAP_SPOT, nullptr, 0, 0},
        {"feedback", TEXTURE_SRC_CODE_FEEDBACK, nullptr, 0, 0},
        {"resolvedPostSun", TEXTURE_SRC_CODE_RESOLVED_POST_SUN, nullptr, 0, 0},
        {"resolvedScene", TEXTURE_SRC_CODE_RESOLVED_SCENE, nullptr, 0, 0},
        {"postEffect0", TEXTURE_SRC_CODE_POST_EFFECT_0, nullptr, 0, 0},
        {"postEffect1", TEXTURE_SRC_CODE_POST_EFFECT_1, nullptr, 0, 0},
        {"light", TEXTURE_SRC_CODE_LIGHT_ATTENUATION, s_lightSamplers, 0, 0},
        {"floatZ", TEXTURE_SRC_CODE_FLOATZ, nullptr, 0, 0},
        {"processedFloatZ", TEXTURE_SRC_CODE_PROCESSED_FLOATZ, nullptr, 0, 0},
        {"rawFloatZ", TEXTURE_SRC_CODE_RAW_FLOATZ, nullptr, 0, 0},
        {"halfParticleColorSampler", TEXTURE_SRC_CODE_HALF_PARTICLES, nullptr, 0, 0},
        {"halfParticleDepthSampler", TEXTURE_SRC_CODE_HALF_PARTICLES_Z, nullptr, 0, 0},
        {"alternateScene", TEXTURE_SRC_CODE_ALTERNATE_SCENE, nullptr, 0, 0},
        {},
    };

    inline CodeSamplerSource s_defaultCodeSamplers[]{
        {"shadowmapSamplerSun", TEXTURE_SRC_CODE_SHADOWMAP_SUN, nullptr, 0, 0},
        {"shadowmapSamplerSpot", TEXTURE_SRC_CODE_SHADOWMAP_SPOT, nullptr, 0, 0},
        {"feedbackSampler", TEXTURE_SRC_CODE_FEEDBACK, nullptr, 0, 0},
        {"floatZSampler", TEXTURE_SRC_CODE_FLOATZ, nullptr, 0, 0},
        {"processedFloatZSampler", TEXTURE_SRC_CODE_PROCESSED_FLOATZ, nullptr, 0, 0},
        {"rawFloatZSampler", TEXTURE_SRC_CODE_RAW_FLOATZ, nullptr, 0, 0},
        {"halfParticleColorSampler", TEXTURE_SRC_CODE_HALF_PARTICLES, nullptr, 0, 0},
        {"halfParticleDepthSampler", TEXTURE_SRC_CODE_HALF_PARTICLES_Z, nullptr, 0, 0},
        {"attenuationSampler", TEXTURE_SRC_CODE_LIGHT_ATTENUATION, nullptr, 0, 0},
        {"lightmapSamplerPrimary", TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY, nullptr, 0, 0},
        {"lightmapSamplerSecondary", TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY, nullptr, 0, 0},
        {"modelLightingSampler", TEXTURE_SRC_CODE_MODEL_LIGHTING, nullptr, 0, 0},
        {"cinematicYSampler", TEXTURE_SRC_CODE_CINEMATIC_Y, nullptr, 0, 0},
        {"cinematicCrSampler", TEXTURE_SRC_CODE_CINEMATIC_CR, nullptr, 0, 0},
        {"cinematicCbSampler", TEXTURE_SRC_CODE_CINEMATIC_CB, nullptr, 0, 0},
        {"cinematicASampler", TEXTURE_SRC_CODE_CINEMATIC_A, nullptr, 0, 0},
        {"reflectionProbeSampler", TEXTURE_SRC_CODE_REFLECTION_PROBE, nullptr, 0, 0},
        {"alternateSceneSampler", TEXTURE_SRC_CODE_ALTERNATE_SCENE, nullptr, 0, 0},
        {},
    };

    inline CodeConstantSource s_sunConsts[]{
        {"position", CONST_SRC_CODE_LIGHT_POSITION, nullptr, 0, 0},
        {"diffuse", CONST_SRC_CODE_LIGHT_DIFFUSE, nullptr, 0, 0},
        {"specular", CONST_SRC_CODE_LIGHT_SPECULAR, nullptr, 0, 0},
        {"spotDir", CONST_SRC_CODE_LIGHT_SPOTDIR, nullptr, 0, 0},
        {"spotFactors", CONST_SRC_CODE_LIGHT_SPOTFACTORS, nullptr, 0, 0},
        {"falloffPlacement", CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT, nullptr, 0, 0},
        {},
    };

    inline CodeConstantSource s_nearPlaneConsts[]{
        {"org", CONST_SRC_CODE_NEARPLANE_ORG, nullptr, 0, 0},
        {"dx", CONST_SRC_CODE_NEARPLANE_DX, nullptr, 0, 0},
        {"dy", CONST_SRC_CODE_NEARPLANE_DY, nullptr, 0, 0},
        {},
    };

    inline CodeConstantSource s_codeConsts[]{
        {"nearPlane", CONST_SRC_NONE, s_nearPlaneConsts, 0, 0},
        {"light", CONST_SRC_NONE, s_sunConsts, 0, 0},
        {"baseLightingCoords", CONST_SRC_CODE_BASE_LIGHTING_COORDS, nullptr, 0, 0},
        {"lightprobeAmbient", CONST_SRC_CODE_LIGHT_PROBE_AMBIENT, nullptr, 0, 0},
        {"fullscreenDistortion", CONST_SRC_CODE_COMPOSITE_FX_DISTORTION, nullptr, 0, 0},
        {"fadeEffect", CONST_SRC_CODE_POSTFX_FADE_EFFECT, nullptr, 0, 0},
        {"lightingLookupScale", CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE, nullptr, 0, 0},
        {"debugBumpmap", CONST_SRC_CODE_DEBUG_BUMPMAP, nullptr, 0, 0},
        {"pixelCostFracs", CONST_SRC_CODE_PIXEL_COST_FRACS, nullptr, 0, 0},
        {"pixelCostDecode", CONST_SRC_CODE_PIXEL_COST_DECODE, nullptr, 0, 0},
        {"materialColor", CONST_SRC_CODE_MATERIAL_COLOR, nullptr, 0, 0},
        {"fogConsts", CONST_SRC_CODE_FOG, nullptr, 0, 0},
        {"fogColorLinear", CONST_SRC_CODE_FOG_COLOR_LINEAR, nullptr, 0, 0},
        {"fogColorGamma", CONST_SRC_CODE_FOG_COLOR_GAMMA, nullptr, 0, 0},
        {"fogSunConsts", CONST_SRC_CODE_FOG_SUN_CONSTS, nullptr, 0, 0},
        {"fogSunColorLinear", CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR, nullptr, 0, 0},
        {"fogSunColorGamma", CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA, nullptr, 0, 0},
        {"fogSunDir", CONST_SRC_CODE_FOG_SUN_DIR, nullptr, 0, 0},
        {"glowSetup", CONST_SRC_CODE_GLOW_SETUP, nullptr, 0, 0},
        {"glowApply", CONST_SRC_CODE_GLOW_APPLY, nullptr, 0, 0},
        {"filterTap", CONST_SRC_CODE_FILTER_TAP_0, nullptr, 8, 1},
        {"codeMeshArg", CONST_SRC_CODE_CODE_MESH_ARG_0, nullptr, 2, 1},
        {"renderTargetSize", CONST_SRC_CODE_RENDER_TARGET_SIZE, nullptr, 0, 0},
        {"shadowmapSwitchPartition", CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION, nullptr, 0, 0},
        {"shadowmapScale", CONST_SRC_CODE_SHADOWMAP_SCALE, nullptr, 0, 0},
        {"shadowmapPolygonOffset", CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET, nullptr, 0, 0},
        {"zNear", CONST_SRC_CODE_ZNEAR, nullptr, 0, 0},
        {"clipSpaceLookupScale", CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE, nullptr, 0, 0},
        {"clipSpaceLookupOffset", CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET, nullptr, 0, 0},
        {"dofEquationViewModelAndFarBlur", CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR, nullptr, 0, 0},
        {"dofEquationScene", CONST_SRC_CODE_DOF_EQUATION_SCENE, nullptr, 0, 0},
        {"dofLerpScale", CONST_SRC_CODE_DOF_LERP_SCALE, nullptr, 0, 0},
        {"dofLerpBias", CONST_SRC_CODE_DOF_LERP_BIAS, nullptr, 0, 0},
        {"dofRowDelta", CONST_SRC_CODE_DOF_ROW_DELTA, nullptr, 0, 0},
        {"depthFromClip", CONST_SRC_CODE_DEPTH_FROM_CLIP, nullptr, 0, 0},
        {"outdoorFeatherParms", CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS, nullptr, 0, 0},
        {"envMapParms", CONST_SRC_CODE_ENVMAP_PARMS, nullptr, 0, 0},
        {"colorMatrixR", CONST_SRC_CODE_COLOR_MATRIX_R, nullptr, 0, 0},
        {"colorMatrixG", CONST_SRC_CODE_COLOR_MATRIX_G, nullptr, 0, 0},
        {"colorMatrixB", CONST_SRC_CODE_COLOR_MATRIX_B, nullptr, 0, 0},
        {"colorBias", CONST_SRC_CODE_COLOR_BIAS, nullptr, 0, 0},
        {"colorTintBase", CONST_SRC_CODE_COLOR_TINT_BASE, nullptr, 0, 0},
        {"colorTintDelta", CONST_SRC_CODE_COLOR_TINT_DELTA, nullptr, 0, 0},
        {"colorTintQuadraticDelta", CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA, nullptr, 0, 0},
        {"motionMatrixX", CONST_SRC_CODE_MOTION_MATRIX_X, nullptr, 0, 0},
        {"motionMatrixY", CONST_SRC_CODE_MOTION_MATRIX_Y, nullptr, 0, 0},
        {"motionMatrixW", CONST_SRC_CODE_MOTION_MATRIX_W, nullptr, 0, 0},
        {"gameTime", CONST_SRC_CODE_GAMETIME, nullptr, 0, 0},
        {"particleCloudColor", CONST_SRC_CODE_PARTICLE_CLOUD_COLOR, nullptr, 0, 0},
        {"particleCloudMatrix", CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0, nullptr, 0, 0},
        {"particleCloudMatrix1", CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1, nullptr, 0, 0},
        {"particleCloudMatrix2", CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2, nullptr, 0, 0},
        {"particleCloudSparkColor0", CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0, nullptr, 0, 0},
        {"particleCloudSparkColor1", CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1, nullptr, 0, 0},
        {"particleCloudSparkColor2", CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2, nullptr, 0, 0},
        {"particleFountainParms0", CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0, nullptr, 0, 0},
        {"particleFountainParms1", CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1, nullptr, 0, 0},
        {"viewportDimensions", CONST_SRC_CODE_VIEWPORT_DIMENSIONS, nullptr, 0, 0},
        {"framebufferRead", CONST_SRC_CODE_FRAMEBUFFER_READ, nullptr, 0, 0},
        {"viewMatrix", CONST_SRC_CODE_VIEW_MATRIX, nullptr, 0, 0},
        {"inverseViewMatrix", CONST_SRC_CODE_INVERSE_VIEW_MATRIX, nullptr, 0, 0},
        {"transposeViewMatrix", CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX, nullptr, 0, 0},
        {"inverseTransposeViewMatrix", CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX, nullptr, 0, 0},
        {"projectionMatrix", CONST_SRC_CODE_PROJECTION_MATRIX, nullptr, 0, 0},
        {"inverseProjectionMatrix", CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX, nullptr, 0, 0},
        {"transposeProjectionMatrix", CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX, nullptr, 0, 0},
        {"inverseTransposeProjectionMatrix", CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX, nullptr, 0, 0},
        {"viewProjectionMatrix", CONST_SRC_CODE_VIEW_PROJECTION_MATRIX, nullptr, 0, 0},
        {"inverseViewProjectionMatrix", CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX, nullptr, 0, 0},
        {"transposeViewProjectionMatrix", CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX, nullptr, 0, 0},
        {"inverseTransposeViewProjectionMatrix", CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX, nullptr, 0, 0},
        {"shadowLookupMatrix", CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX, nullptr, 0, 0},
        {"inverseShadowLookupMatrix", CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX, nullptr, 0, 0},
        {"transposeShadowLookupMatrix", CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, nullptr, 0, 0},
        {"inverseTransposeShadowLookupMatrix", CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, nullptr, 0, 0},
        {"worldOutdoorLookupMatrix", CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX, nullptr, 0, 0},
        {"inverseWorldOutdoorLookupMatrix", CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX, nullptr, 0, 0},
        {"transposeWorldOutdoorLookupMatrix", CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX, nullptr, 0, 0},
        {"inverseTransposeWorldOutdoorLookupMatrix", CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX, nullptr, 0, 0},
        {"worldMatrix", CONST_SRC_CODE_WORLD_MATRIX0, nullptr, 0, 0},
        {"inverseWorldMatrix", CONST_SRC_CODE_INVERSE_WORLD_MATRIX0, nullptr, 0, 0},
        {"transposeWorldMatrix", CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0, nullptr, 0, 0},
        {"inverseTransposeWorldMatrix", CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0, nullptr, 0, 0},
        {"worldViewMatrix", CONST_SRC_CODE_WORLD_VIEW_MATRIX0, nullptr, 0, 0},
        {"inverseWorldViewMatrix", CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0, nullptr, 0, 0},
        {"transposeWorldViewMatrix", CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0, nullptr, 0, 0},
        {"inverseTransposeWorldViewMatrix", CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0, nullptr, 0, 0},
        {"worldViewProjectionMatrix", CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0, nullptr, 0, 0},
        {"inverseWorldViewProjectionMatrix", CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0, nullptr, 0, 0},
        {"transposeWorldViewProjectionMatrix", CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0, nullptr, 0, 0},
        {"inverseTransposeWorldViewProjectionMatrix", CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0, nullptr, 0, 0},
        {"worldMatrix1", CONST_SRC_CODE_WORLD_MATRIX1, nullptr, 0, 0},
        {"inverseWorldMatrix1", CONST_SRC_CODE_INVERSE_WORLD_MATRIX1, nullptr, 0, 0},
        {"transposeWorldMatrix1", CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1, nullptr, 0, 0},
        {"inverseTransposeWorldMatrix1", CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1, nullptr, 0, 0},
        {"worldViewMatrix1", CONST_SRC_CODE_WORLD_VIEW_MATRIX1, nullptr, 0, 0},
        {"inverseWorldViewMatrix1", CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1, nullptr, 0, 0},
        {"transposeWorldViewMatrix1", CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1, nullptr, 0, 0},
        {"inverseTransposeWorldViewMatrix1", CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1, nullptr, 0, 0},
        {"worldViewProjectionMatrix1", CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1, nullptr, 0, 0},
        {"inverseWorldViewProjectionMatrix1", CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1, nullptr, 0, 0},
        {"transposeWorldViewProjectionMatrix1", CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1, nullptr, 0, 0},
        {"inverseTransposeWorldViewProjectionMatrix1", CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1, nullptr, 0, 0},
        {"worldMatrix2", CONST_SRC_CODE_WORLD_MATRIX2, nullptr, 0, 0},
        {"inverseWorldMatrix2", CONST_SRC_CODE_INVERSE_WORLD_MATRIX2, nullptr, 0, 0},
        {"transposeWorldMatrix2", CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2, nullptr, 0, 0},
        {"inverseTransposeWorldMatrix2", CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2, nullptr, 0, 0},
        {"worldViewMatrix2", CONST_SRC_CODE_WORLD_VIEW_MATRIX2, nullptr, 0, 0},
        {"inverseWorldViewMatrix2", CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2, nullptr, 0, 0},
        {"transposeWorldViewMatrix2", CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2, nullptr, 0, 0},
        {"inverseTransposeWorldViewMatrix2", CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2, nullptr, 0, 0},
        {"worldViewProjectionMatrix2", CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2, nullptr, 0, 0},
        {"inverseWorldViewProjectionMatrix2", CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2, nullptr, 0, 0},
        {"transposeWorldViewProjectionMatrix2", CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2, nullptr, 0, 0},
        {"inverseTransposeWorldViewProjectionMatrix2", CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2, nullptr, 0, 0},
        {},
    };

    inline CodeConstantSource s_defaultCodeConsts[]{
        {"nearPlaneOrg", CONST_SRC_CODE_NEARPLANE_ORG, nullptr, 0, 0},
        {"nearPlaneDx", CONST_SRC_CODE_NEARPLANE_DX, nullptr, 0, 0},
        {"nearPlaneDy", CONST_SRC_CODE_NEARPLANE_DY, nullptr, 0, 0},
        {"lightPosition", CONST_SRC_CODE_LIGHT_POSITION, nullptr, 0, 0},
        {"lightDiffuse", CONST_SRC_CODE_LIGHT_DIFFUSE, nullptr, 0, 0},
        {"lightSpecular", CONST_SRC_CODE_LIGHT_SPECULAR, nullptr, 0, 0},
        {"lightSpotDir", CONST_SRC_CODE_LIGHT_SPOTDIR, nullptr, 0, 0},
        {"lightSpotFactors", CONST_SRC_CODE_LIGHT_SPOTFACTORS, nullptr, 0, 0},
        {"lightFalloffPlacement", CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT, nullptr, 0, 0},
        {"sunShadowmapPixelAdjust", CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST, nullptr, 0, 0},
        {"spotShadowmapPixelAdjust", CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST, nullptr, 0, 0},
        {},
    };

    inline MaterialUpdateFrequency s_codeConstUpdateFreq[]{
        MTL_UPDATE_RARELY,     // LIGHT_POSITION
        MTL_UPDATE_RARELY,     // LIGHT_DIFFUSE
        MTL_UPDATE_RARELY,     // LIGHT_SPECULAR
        MTL_UPDATE_RARELY,     // LIGHT_SPOTDIR
        MTL_UPDATE_RARELY,     // LIGHT_SPOTFACTORS
        MTL_UPDATE_RARELY,     // LIGHT_FALLOFF_PLACEMENT
        MTL_UPDATE_RARELY,     // PARTICLE_CLOUD_COLOR
        MTL_UPDATE_RARELY,     // GAMETIME
        MTL_UPDATE_RARELY,     // PIXEL_COST_FRACS
        MTL_UPDATE_RARELY,     // PIXEL_COST_DECODE
        MTL_UPDATE_RARELY,     // FILTER_TAP_0
        MTL_UPDATE_RARELY,     // FILTER_TAP_1
        MTL_UPDATE_RARELY,     // FILTER_TAP_2
        MTL_UPDATE_RARELY,     // FILTER_TAP_3
        MTL_UPDATE_RARELY,     // FILTER_TAP_4
        MTL_UPDATE_RARELY,     // FILTER_TAP_5
        MTL_UPDATE_RARELY,     // FILTER_TAP_6
        MTL_UPDATE_RARELY,     // FILTER_TAP_7
        MTL_UPDATE_RARELY,     // COLOR_MATRIX_R
        MTL_UPDATE_RARELY,     // COLOR_MATRIX_G
        MTL_UPDATE_RARELY,     // COLOR_MATRIX_B
        MTL_UPDATE_RARELY,     // SHADOWMAP_POLYGON_OFFSET
        MTL_UPDATE_RARELY,     // RENDER_TARGET_SIZE
        MTL_UPDATE_RARELY,     // DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR
        MTL_UPDATE_RARELY,     // DOF_EQUATION_SCENE
        MTL_UPDATE_RARELY,     // DOF_LERP_SCALE
        MTL_UPDATE_RARELY,     // DOF_LERP_BIAS
        MTL_UPDATE_RARELY,     // DOF_ROW_DELTA
        MTL_UPDATE_RARELY,     // MOTION_MATRIX_X
        MTL_UPDATE_RARELY,     // MOTION_MATRIX_Y
        MTL_UPDATE_RARELY,     // MOTION_MATRIX_W
        MTL_UPDATE_RARELY,     // SHADOWMAP_SWITCH_PARTITION
        MTL_UPDATE_RARELY,     // SHADOWMAP_SCALE
        MTL_UPDATE_RARELY,     // ZNEAR
        MTL_UPDATE_RARELY,     // LIGHTING_LOOKUP_SCALE
        MTL_UPDATE_RARELY,     // DEBUG_BUMPMAP
        MTL_UPDATE_RARELY,     // MATERIAL_COLOR
        MTL_UPDATE_RARELY,     // FOG
        MTL_UPDATE_RARELY,     // FOG_COLOR_LINEAR
        MTL_UPDATE_RARELY,     // FOG_COLOR_GAMMA
        MTL_UPDATE_RARELY,     // FOG_SUN_CONSTS
        MTL_UPDATE_RARELY,     // FOG_SUN_COLOR_LINEAR
        MTL_UPDATE_RARELY,     // FOG_SUN_COLOR_GAMMA
        MTL_UPDATE_RARELY,     // FOG_SUN_DIR
        MTL_UPDATE_RARELY,     // GLOW_SETUP
        MTL_UPDATE_RARELY,     // GLOW_APPLY
        MTL_UPDATE_RARELY,     // COLOR_BIAS
        MTL_UPDATE_RARELY,     // COLOR_TINT_BASE
        MTL_UPDATE_RARELY,     // COLOR_TINT_DELTA
        MTL_UPDATE_RARELY,     // COLOR_TINT_QUADRATIC_DELTA
        MTL_UPDATE_RARELY,     // OUTDOOR_FEATHER_PARMS
        MTL_UPDATE_RARELY,     // ENVMAP_PARMS
        MTL_UPDATE_RARELY,     // SUN_SHADOWMAP_PIXEL_ADJUST
        MTL_UPDATE_RARELY,     // SPOT_SHADOWMAP_PIXEL_ADJUST
        MTL_UPDATE_RARELY,     // COMPOSITE_FX_DISTORTION
        MTL_UPDATE_RARELY,     // POSTFX_FADE_EFFECT
        MTL_UPDATE_RARELY,     // VIEWPORT_DIMENSIONS
        MTL_UPDATE_RARELY,     // FRAMEBUFFER_READ
        MTL_UPDATE_PER_PRIM,   // BASE_LIGHTING_COORDS
        MTL_UPDATE_PER_PRIM,   // LIGHT_PROBE_AMBIENT
        MTL_UPDATE_RARELY,     // NEARPLANE_ORG
        MTL_UPDATE_RARELY,     // NEARPLANE_DX
        MTL_UPDATE_RARELY,     // NEARPLANE_DY
        MTL_UPDATE_RARELY,     // CLIP_SPACE_LOOKUP_SCALE
        MTL_UPDATE_RARELY,     // CLIP_SPACE_LOOKUP_OFFSET
        MTL_UPDATE_PER_OBJECT, // PARTICLE_CLOUD_MATRIX0
        MTL_UPDATE_PER_OBJECT, // PARTICLE_CLOUD_MATRIX1
        MTL_UPDATE_PER_OBJECT, // PARTICLE_CLOUD_MATRIX2
        MTL_UPDATE_PER_OBJECT, // PARTICLE_CLOUD_SPARK_COLOR0
        MTL_UPDATE_PER_OBJECT, // PARTICLE_CLOUD_SPARK_COLOR1
        MTL_UPDATE_PER_OBJECT, // PARTICLE_CLOUD_SPARK_COLOR2
        MTL_UPDATE_PER_OBJECT, // PARTICLE_FOUNTAIN_PARM0
        MTL_UPDATE_PER_OBJECT, // PARTICLE_FOUNTAIN_PARM1
        MTL_UPDATE_PER_OBJECT, // DEPTH_FROM_CLIP
        MTL_UPDATE_PER_OBJECT, // CODE_MESH_ARG_0
        MTL_UPDATE_PER_OBJECT, // CODE_MESH_ARG_1
        MTL_UPDATE_PER_OBJECT, // VIEW_MATRIX
        MTL_UPDATE_PER_OBJECT, // INVERSE_VIEW_MATRIX
        MTL_UPDATE_PER_OBJECT, // TRANSPOSE_VIEW_MATRIX
        MTL_UPDATE_PER_OBJECT, // INVERSE_TRANSPOSE_VIEW_MATRIX
        MTL_UPDATE_PER_OBJECT, // PROJECTION_MATRIX
        MTL_UPDATE_PER_OBJECT, // INVERSE_PROJECTION_MATRIX
        MTL_UPDATE_PER_OBJECT, // TRANSPOSE_PROJECTION_MATRIX
        MTL_UPDATE_PER_OBJECT, // INVERSE_TRANSPOSE_PROJECTION_MATRIX
        MTL_UPDATE_PER_OBJECT, // VIEW_PROJECTION_MATRIX
        MTL_UPDATE_PER_OBJECT, // INVERSE_VIEW_PROJECTION_MATRIX
        MTL_UPDATE_PER_OBJECT, // TRANSPOSE_VIEW_PROJECTION_MATRIX
        MTL_UPDATE_PER_OBJECT, // INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX
        MTL_UPDATE_PER_OBJECT, // SHADOW_LOOKUP_MATRIX
        MTL_UPDATE_PER_OBJECT, // INVERSE_SHADOW_LOOKUP_MATRIX
        MTL_UPDATE_PER_OBJECT, // TRANSPOSE_SHADOW_LOOKUP_MATRIX
        MTL_UPDATE_PER_OBJECT, // INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX
        MTL_UPDATE_PER_PRIM,   // WORLD_OUTDOOR_LOOKUP_MATRIX
        MTL_UPDATE_PER_PRIM,   // INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX
        MTL_UPDATE_PER_PRIM,   // TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX
        MTL_UPDATE_PER_PRIM,   // INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX
        MTL_UPDATE_PER_PRIM,   // WORLD_MATRIX0
        MTL_UPDATE_PER_PRIM,   // INVERSE_WORLD_MATRIX0
        MTL_UPDATE_PER_PRIM,   // TRANSPOSE_WORLD_MATRIX0
        MTL_UPDATE_PER_PRIM,   // INVERSE_TRANSPOSE_WORLD_MATRIX0
        MTL_UPDATE_PER_PRIM,   // WORLD_VIEW_MATRIX0
        MTL_UPDATE_PER_PRIM,   // INVERSE_WORLD_VIEW_MATRIX0
        MTL_UPDATE_PER_PRIM,   // TRANSPOSE_WORLD_VIEW_MATRIX0
        MTL_UPDATE_PER_PRIM,   // INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0
        MTL_UPDATE_PER_PRIM,   // WORLD_VIEW_PROJECTION_MATRIX0
        MTL_UPDATE_PER_PRIM,   // INVERSE_WORLD_VIEW_PROJECTION_MATRIX0
        MTL_UPDATE_PER_PRIM,   // TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0
        MTL_UPDATE_PER_PRIM,   // INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0
        MTL_UPDATE_PER_PRIM,   // WORLD_MATRIX1
        MTL_UPDATE_PER_PRIM,   // INVERSE_WORLD_MATRIX1
        MTL_UPDATE_PER_PRIM,   // TRANSPOSE_WORLD_MATRIX1
        MTL_UPDATE_PER_PRIM,   // INVERSE_TRANSPOSE_WORLD_MATRIX1
        MTL_UPDATE_PER_PRIM,   // WORLD_VIEW_MATRIX1
        MTL_UPDATE_PER_PRIM,   // INVERSE_WORLD_VIEW_MATRIX1
        MTL_UPDATE_PER_PRIM,   // TRANSPOSE_WORLD_VIEW_MATRIX1
        MTL_UPDATE_PER_PRIM,   // INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1
        MTL_UPDATE_PER_PRIM,   // WORLD_VIEW_PROJECTION_MATRIX1
        MTL_UPDATE_PER_PRIM,   // INVERSE_WORLD_VIEW_PROJECTION_MATRIX1
        MTL_UPDATE_PER_PRIM,   // TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1
        MTL_UPDATE_PER_PRIM,   // INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1
        MTL_UPDATE_PER_PRIM,   // WORLD_MATRIX2
        MTL_UPDATE_PER_PRIM,   // INVERSE_WORLD_MATRIX2
        MTL_UPDATE_PER_PRIM,   // TRANSPOSE_WORLD_MATRIX2
        MTL_UPDATE_PER_PRIM,   // INVERSE_TRANSPOSE_WORLD_MATRIX2
        MTL_UPDATE_PER_PRIM,   // WORLD_VIEW_MATRIX2
        MTL_UPDATE_PER_PRIM,   // INVERSE_WORLD_VIEW_MATRIX2
        MTL_UPDATE_PER_PRIM,   // TRANSPOSE_WORLD_VIEW_MATRIX2
        MTL_UPDATE_PER_PRIM,   // INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2
        MTL_UPDATE_PER_PRIM,   // WORLD_VIEW_PROJECTION_MATRIX2
        MTL_UPDATE_PER_PRIM,   // INVERSE_WORLD_VIEW_PROJECTION_MATRIX2
        MTL_UPDATE_PER_PRIM,   // TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2
        MTL_UPDATE_PER_PRIM,   // INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2
    };
    static_assert(std::extent_v<decltype(s_codeConstUpdateFreq)> == CONST_SRC_TOTAL_COUNT);

    inline MaterialUpdateFrequency s_codeSamplerUpdateFreq[]{
        MTL_UPDATE_RARELY,     // BLACK
        MTL_UPDATE_RARELY,     // WHITE
        MTL_UPDATE_RARELY,     // IDENTITY_NORMAL_MAP
        MTL_UPDATE_RARELY,     // MODEL_LIGHTING
        MTL_UPDATE_CUSTOM,     // LIGHTMAP_PRIMARY
        MTL_UPDATE_CUSTOM,     // LIGHTMAP_SECONDARY
        MTL_UPDATE_RARELY,     // SHADOWMAP_SUN
        MTL_UPDATE_RARELY,     // SHADOWMAP_SPOT
        MTL_UPDATE_PER_OBJECT, // FEEDBACK
        MTL_UPDATE_RARELY,     // RESOLVED_POST_SUN
        MTL_UPDATE_RARELY,     // RESOLVED_SCENE
        MTL_UPDATE_RARELY,     // POST_EFFECT_0
        MTL_UPDATE_RARELY,     // POST_EFFECT_1
        MTL_UPDATE_PER_OBJECT, // LIGHT_ATTENUATION
        MTL_UPDATE_RARELY,     // OUTDOOR
        MTL_UPDATE_RARELY,     // FLOATZ
        MTL_UPDATE_RARELY,     // PROCESSED_FLOATZ
        MTL_UPDATE_RARELY,     // RAW_FLOATZ
        MTL_UPDATE_RARELY,     // HALF_PARTICLES
        MTL_UPDATE_RARELY,     // HALF_PARTICLES_Z
        MTL_UPDATE_PER_OBJECT, // CASE_TEXTURE
        MTL_UPDATE_PER_OBJECT, // CINEMATIC_Y
        MTL_UPDATE_PER_OBJECT, // CINEMATIC_CR
        MTL_UPDATE_PER_OBJECT, // CINEMATIC_CB
        MTL_UPDATE_PER_OBJECT, // CINEMATIC_A
        MTL_UPDATE_CUSTOM,     // REFLECTION_PROBE
        MTL_UPDATE_RARELY,     // ALTERNATE_SCENE
    };
    static_assert(std::extent_v<decltype(s_codeSamplerUpdateFreq)> == TEXTURE_SRC_CODE_COUNT);

    inline MaterialTextureSource g_customSamplerSrc[]{
        TEXTURE_SRC_CODE_REFLECTION_PROBE,   // CUSTOM_SAMPLER_REFLECTION_PROBE
        TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY,   // CUSTOM_SAMPLER_LIGHTMAP_PRIMARY
        TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY, // CUSTOM_SAMPLER_LIGHTMAP_SECONDARY
    };
    static_assert(std::extent_v<decltype(g_customSamplerSrc)> == CUSTOM_SAMPLER_COUNT);

    inline MaterialTypeInfo g_materialTypeInfo[]{
        {"",    ""   },
        {"m/",  "m_" },
        {"mc/", "mc_"},
        {"mg/", "mg_"},
        {"w/",  "w_" },
        {"wc/", "wc_"},
    };
    static_assert(std::extent_v<decltype(g_materialTypeInfo)> == MTL_TYPE_COUNT);

    struct KnownMaterialTextureMap
    {
        const char* m_name;
        const char* m_additional_property_suffix;
    };

    static constexpr std::pair<uint32_t, KnownMaterialTextureMap> MakeKnownTextureMap(const char* name, const char* additionalPropertySuffix)
    {
        return std::make_pair(Common::R_HashString(name, 0u), KnownMaterialTextureMap{name, additionalPropertySuffix});
    }

    inline std::unordered_map knownTextureMaps{
        MakeKnownTextureMap("colorMap", "Color"),
        MakeKnownTextureMap("colorMap0", "Color00"),
        MakeKnownTextureMap("colorMap1", "Color01"),
        MakeKnownTextureMap("colorMap2", "Color02"),
        MakeKnownTextureMap("normalMap", "Normal"),
        MakeKnownTextureMap("specularMap", "Specular"),
        MakeKnownTextureMap("detailMap", "Detail"),
    };

    static constexpr std::pair<uint32_t, const char*> MakeKnownConstantName(const char* name)
    {
        return std::make_pair(Common::R_HashString(name, 0u), name);
    }

    inline std::unordered_map knownConstantNames{
        MakeKnownConstantName("distortionScale"),
        MakeKnownConstantName("eyeOffsetParms"),
        MakeKnownConstantName("falloffBeginColor"),
        MakeKnownConstantName("falloffEndColor"),
    };

    enum class StateMapLayout_e
    {
        ALPHA_TEST,
        BLEND_FUNC,
        SEPARATE_ALPHA_BLEND_FUNC,
        CULL_FACE,
        DEPTH_TEST,
        DEPTH_WRITE,
        COLOR_WRITE,
        GAMMA_WRITE,
        POLYGON_OFFSET,
        STENCIL,
        WIREFRAME
    };

    inline state_map::StateMapLayoutEntries stateMapEntryLayout({
        {"alphaTest",              0, GFXS0_ATEST_MASK | GFXS0_ATEST_DISABLE,                            {"mtlAlphaTest"}                                            },
        {"blendFunc",              0, GFXS0_BLEND_RGB_MASK,                                              {"mtlBlendOp", "mtlSrcBlend", "mtlDestBlend"}               },
        {"separateAlphaBlendFunc", 0, GFXS0_BLEND_ALPHA_MASK,                                            {"mtlBlendOpAlpha", "mtlSrcBlendAlpha", "mtlDestBlendAlpha"}},
        {"cullFace",               0, GFXS0_CULL_MASK,                                                   {"mtlCullFace"}                                             },
        {"depthTest",              1, GFXS1_DEPTHTEST_MASK | GFXS1_DEPTHTEST_DISABLE,                    {"mtlDepthTest"}                                            },
        {"depthWrite",             1, GFXS1_DEPTHWRITE,                                                  {"mtlDepthWrite"}                                           },
        {"colorWrite",             0, GFXS0_COLORWRITE_MASK,                                             {"mtlColorWriteRgb", "mtlColorWriteAlpha"}                  },
        {"gammaWrite",
         0,                           GFXS0_GAMMAWRITE,
         {
             "mtlGammaWrite",
         }                                                                                                                                                           },
        {"polygonOffset",
         1,                           GFXS1_POLYGON_OFFSET_MASK,
         {
             "mtlPolygonOffset",
         }                                                                                                                                                           },
        {"stencil",
         1,                           GFXS1_STENCILFUNC_FRONTBACK_MASK | GFXS1_STENCILOP_FRONTBACK_MASK,
         {
             "mtlStencil",
             "mtlStencilFuncFront",
             "mtlStencilOpFrontPass",
             "mtlStencilOpFrontFail",
             "mtlStencilOpFrontZFail",
             "mtlStencilFuncBack",
             "mtlStencilOpBackPass",
             "mtlStencilOpBackFail",
             "mtlStencilOpBackZFail",
         }                                                                                                                                                           },
        {"wireframe",
         0,                           GFXS0_POLYMODE_LINE,
         {
             "mtlWireframe",
         }                                                                                                                                                           }
    });

    inline state_map::StateMapLayoutVars stateMapVarLayout({
        {"mtlAlphaTest",
         0, {
             {"Always", GFXS0_ATEST_DISABLE},
             {"GE128", GFXS0_ATEST_GE_128},
             {"GT0", GFXS0_ATEST_GT_0},
             {"LT128", GFXS0_ATEST_LT_128},
         }},
        {"mtlBlendOp",
         0, {
             {"Add", GFXS_BLENDOP_ADD << GFXS0_BLENDOP_RGB_SHIFT},
             {"Subtract", GFXS_BLENDOP_SUBTRACT << GFXS0_BLENDOP_RGB_SHIFT},
             {"RevSubtract", GFXS_BLENDOP_REVSUBTRACT << GFXS0_BLENDOP_RGB_SHIFT},
             {"Min", GFXS_BLENDOP_MIN << GFXS0_BLENDOP_RGB_SHIFT},
             {"Max", GFXS_BLENDOP_MAX << GFXS0_BLENDOP_RGB_SHIFT},
             {"Disable", GFXS_BLENDOP_DISABLED << GFXS0_BLENDOP_RGB_SHIFT},
         }},
        {"mtlSrcBlend",
         0, {
             {"Zero", GFXS_BLEND_ZERO << GFXS0_SRCBLEND_RGB_SHIFT},
             {"One", GFXS_BLEND_ONE << GFXS0_SRCBLEND_RGB_SHIFT},
             {"SrcColor", GFXS_BLEND_SRCCOLOR << GFXS0_SRCBLEND_RGB_SHIFT},
             {"InvSrcColor", GFXS_BLEND_INVSRCCOLOR << GFXS0_SRCBLEND_RGB_SHIFT},
             {"SrcAlpha", GFXS_BLEND_SRCALPHA << GFXS0_SRCBLEND_RGB_SHIFT},
             {"InvSrcAlpha", GFXS_BLEND_INVSRCALPHA << GFXS0_SRCBLEND_RGB_SHIFT},
             {"DestAlpha", GFXS_BLEND_DESTALPHA << GFXS0_SRCBLEND_RGB_SHIFT},
             {"InvDestAlpha", GFXS_BLEND_INVDESTALPHA << GFXS0_SRCBLEND_RGB_SHIFT},
             {"DestColor", GFXS_BLEND_DESTCOLOR << GFXS0_SRCBLEND_RGB_SHIFT},
             {"InvDestColor", GFXS_BLEND_INVDESTCOLOR << GFXS0_SRCBLEND_RGB_SHIFT},
         }},
        {"mtlDestBlend",
         0, {
             {"Zero", GFXS_BLEND_ZERO << GFXS0_DSTBLEND_RGB_SHIFT},
             {"One", GFXS_BLEND_ONE << GFXS0_DSTBLEND_RGB_SHIFT},
             {"SrcColor", GFXS_BLEND_SRCCOLOR << GFXS0_DSTBLEND_RGB_SHIFT},
             {"InvSrcColor", GFXS_BLEND_INVSRCCOLOR << GFXS0_DSTBLEND_RGB_SHIFT},
             {"SrcAlpha", GFXS_BLEND_SRCALPHA << GFXS0_DSTBLEND_RGB_SHIFT},
             {"InvSrcAlpha", GFXS_BLEND_INVSRCALPHA << GFXS0_DSTBLEND_RGB_SHIFT},
             {"DestAlpha", GFXS_BLEND_DESTALPHA << GFXS0_DSTBLEND_RGB_SHIFT},
             {"InvDestAlpha", GFXS_BLEND_INVDESTALPHA << GFXS0_DSTBLEND_RGB_SHIFT},
             {"DestColor", GFXS_BLEND_DESTCOLOR << GFXS0_DSTBLEND_RGB_SHIFT},
             {"InvDestColor", GFXS_BLEND_INVDESTCOLOR << GFXS0_DSTBLEND_RGB_SHIFT},
         }},
        {"mtlBlendOpAlpha",
         0, {
             {"Add", GFXS_BLENDOP_ADD << GFXS0_BLENDOP_ALPHA_SHIFT},
             {"Subtract", GFXS_BLENDOP_SUBTRACT << GFXS0_BLENDOP_ALPHA_SHIFT},
             {"RevSubtract", GFXS_BLENDOP_REVSUBTRACT << GFXS0_BLENDOP_ALPHA_SHIFT},
             {"Min", GFXS_BLENDOP_MIN << GFXS0_BLENDOP_ALPHA_SHIFT},
             {"Max", GFXS_BLENDOP_MAX << GFXS0_BLENDOP_ALPHA_SHIFT},
             {"Disable", GFXS_BLENDOP_DISABLED << GFXS0_BLENDOP_ALPHA_SHIFT},
         }},
        {"mtlSrcBlendAlpha",
         0, {
             {"Zero", GFXS_BLEND_ZERO << GFXS0_SRCBLEND_ALPHA_SHIFT},
             {"One", GFXS_BLEND_ONE << GFXS0_SRCBLEND_ALPHA_SHIFT},
             {"SrcColor", GFXS_BLEND_SRCCOLOR << GFXS0_SRCBLEND_ALPHA_SHIFT},
             {"InvSrcColor", GFXS_BLEND_INVSRCCOLOR << GFXS0_SRCBLEND_ALPHA_SHIFT},
             {"SrcAlpha", GFXS_BLEND_SRCALPHA << GFXS0_SRCBLEND_ALPHA_SHIFT},
             {"InvSrcAlpha", GFXS_BLEND_INVSRCALPHA << GFXS0_SRCBLEND_ALPHA_SHIFT},
             {"DestAlpha", GFXS_BLEND_DESTALPHA << GFXS0_SRCBLEND_ALPHA_SHIFT},
             {"InvDestAlpha", GFXS_BLEND_INVDESTALPHA << GFXS0_SRCBLEND_ALPHA_SHIFT},
             {"DestColor", GFXS_BLEND_DESTCOLOR << GFXS0_SRCBLEND_ALPHA_SHIFT},
             {"InvDestColor", GFXS_BLEND_INVDESTCOLOR << GFXS0_SRCBLEND_ALPHA_SHIFT},
         }},
        {"mtlDestBlendAlpha",
         0, {
             {"Zero", GFXS_BLEND_ZERO << GFXS0_DSTBLEND_ALPHA_SHIFT},
             {"One", GFXS_BLEND_ONE << GFXS0_DSTBLEND_ALPHA_SHIFT},
             {"SrcColor", GFXS_BLEND_SRCCOLOR << GFXS0_DSTBLEND_ALPHA_SHIFT},
             {"InvSrcColor", GFXS_BLEND_INVSRCCOLOR << GFXS0_DSTBLEND_ALPHA_SHIFT},
             {"SrcAlpha", GFXS_BLEND_SRCALPHA << GFXS0_DSTBLEND_ALPHA_SHIFT},
             {"InvSrcAlpha", GFXS_BLEND_INVSRCALPHA << GFXS0_DSTBLEND_ALPHA_SHIFT},
             {"DestAlpha", GFXS_BLEND_DESTALPHA << GFXS0_DSTBLEND_ALPHA_SHIFT},
             {"InvDestAlpha", GFXS_BLEND_INVDESTALPHA << GFXS0_DSTBLEND_ALPHA_SHIFT},
             {"DestColor", GFXS_BLEND_DESTCOLOR << GFXS0_DSTBLEND_ALPHA_SHIFT},
             {"InvDestColor", GFXS_BLEND_INVDESTCOLOR << GFXS0_DSTBLEND_ALPHA_SHIFT},
         }},
        {"mtlCullFace",
         0, {
             {"None", GFXS0_CULL_NONE},
             {"Back", GFXS0_CULL_BACK},
             {"Front", GFXS0_CULL_FRONT},
         }},
        {"mtlColorWriteRgb",
         0, {
             {"Enable", GFXS0_COLORWRITE_RGB},
             {"Disable", 0},
         }},
        {"mtlColorWriteAlpha",
         0, {
             {"Enable", GFXS0_COLORWRITE_ALPHA},
             {"Disable", 0},
         }},
        {"mtlGammaWrite",
         0, {
             {"Enable", GFXS0_GAMMAWRITE},
             {"Disable", 0},
         }},
        {"mtlWireframe",
         0, {
             {"Enable", GFXS0_POLYMODE_LINE},
             {"Disable", 0},
         }},
        {"mtlDepthTest",
         1, {
             {"Disable", GFXS1_DEPTHTEST_DISABLE},
             {"Less", GFXS1_DEPTHTEST_LESS},
             {"LessEqual", GFXS1_DEPTHTEST_LESSEQUAL},
             {"Equal", GFXS1_DEPTHTEST_EQUAL},
             {"Always", GFXS1_DEPTHTEST_ALWAYS},
         }},
        {"mtlDepthWrite",
         1, {
             {"Enable", GFXS1_DEPTHWRITE},
             {"Disable", 0},
         }},
        {"mtlPolygonOffset",
         1, {
             {"1", GFXS1_POLYGON_OFFSET_1},
             {"2", GFXS1_POLYGON_OFFSET_2},
             {"shadowmap", GFXS1_POLYGON_OFFSET_SHADOWMAP},
             {"0", GFXS1_POLYGON_OFFSET_0},
         }},
        {"mtlStencil",
         1, {
             {"OneSided", GFXS1_STENCIL_FRONT_ENABLE},
             {"TwoSided", GFXS1_STENCIL_FRONT_ENABLE | GFXS1_STENCIL_BACK_ENABLE},
             {"Disable", 0},
         }},
        {"mtlStencilFuncFront",
         1, {
             {"Less", GFXS_STENCILFUNC_LESS << GFXS1_STENCIL_FRONT_FUNC_SHIFT},
             {"Equal", GFXS_STENCILFUNC_EQUAL << GFXS1_STENCIL_FRONT_FUNC_SHIFT},
             {"LessEqual", GFXS_STENCILFUNC_LESSEQUAL << GFXS1_STENCIL_FRONT_FUNC_SHIFT},
             {"Greater", GFXS_STENCILFUNC_GREATER << GFXS1_STENCIL_FRONT_FUNC_SHIFT},
             {"NotEqual", GFXS_STENCILFUNC_NOTEQUAL << GFXS1_STENCIL_FRONT_FUNC_SHIFT},
             {"GreaterEqual", GFXS_STENCILFUNC_GREATEREQUAL << GFXS1_STENCIL_FRONT_FUNC_SHIFT},
             {"Always", GFXS_STENCILFUNC_ALWAYS << GFXS1_STENCIL_FRONT_FUNC_SHIFT},
             {"Never", GFXS_STENCILFUNC_NEVER << GFXS1_STENCIL_FRONT_FUNC_SHIFT},
         }},
        {"mtlStencilOpFrontPass",
         1, {
             {"Zero", GFXS_STENCILOP_ZERO << GFXS1_STENCIL_FRONT_PASS_SHIFT},
             {"Replace", GFXS_STENCILOP_REPLACE << GFXS1_STENCIL_FRONT_PASS_SHIFT},
             {"IncrSat", GFXS_STENCILOP_INCRSAT << GFXS1_STENCIL_FRONT_PASS_SHIFT},
             {"DecrSat", GFXS_STENCILOP_DECRSAT << GFXS1_STENCIL_FRONT_PASS_SHIFT},
             {"Invert", GFXS_STENCILOP_INVERT << GFXS1_STENCIL_FRONT_PASS_SHIFT},
             {"Incr", GFXS_STENCILOP_INCR << GFXS1_STENCIL_FRONT_PASS_SHIFT},
             {"Decr", GFXS_STENCILOP_DECR << GFXS1_STENCIL_FRONT_PASS_SHIFT},
             {"Keep", GFXS_STENCILOP_KEEP << GFXS1_STENCIL_FRONT_PASS_SHIFT},
         }},
        {"mtlStencilOpFrontFail",
         1, {
             {"Zero", GFXS_STENCILOP_ZERO << GFXS1_STENCIL_FRONT_FAIL_SHIFT},
             {"Replace", GFXS_STENCILOP_REPLACE << GFXS1_STENCIL_FRONT_FAIL_SHIFT},
             {"IncrSat", GFXS_STENCILOP_INCRSAT << GFXS1_STENCIL_FRONT_FAIL_SHIFT},
             {"DecrSat", GFXS_STENCILOP_DECRSAT << GFXS1_STENCIL_FRONT_FAIL_SHIFT},
             {"Invert", GFXS_STENCILOP_INVERT << GFXS1_STENCIL_FRONT_FAIL_SHIFT},
             {"Incr", GFXS_STENCILOP_INCR << GFXS1_STENCIL_FRONT_FAIL_SHIFT},
             {"Decr", GFXS_STENCILOP_DECR << GFXS1_STENCIL_FRONT_FAIL_SHIFT},
             {"Keep", GFXS_STENCILOP_KEEP << GFXS1_STENCIL_FRONT_FAIL_SHIFT},
         }},
        {"mtlStencilOpFrontZFail",
         1, {
             {"Zero", GFXS_STENCILOP_ZERO << GFXS1_STENCIL_FRONT_ZFAIL_SHIFT},
             {"Replace", GFXS_STENCILOP_REPLACE << GFXS1_STENCIL_FRONT_ZFAIL_SHIFT},
             {"IncrSat", GFXS_STENCILOP_INCRSAT << GFXS1_STENCIL_FRONT_ZFAIL_SHIFT},
             {"DecrSat", GFXS_STENCILOP_DECRSAT << GFXS1_STENCIL_FRONT_ZFAIL_SHIFT},
             {"Invert", GFXS_STENCILOP_INVERT << GFXS1_STENCIL_FRONT_ZFAIL_SHIFT},
             {"Incr", GFXS_STENCILOP_INCR << GFXS1_STENCIL_FRONT_ZFAIL_SHIFT},
             {"Decr", GFXS_STENCILOP_DECR << GFXS1_STENCIL_FRONT_ZFAIL_SHIFT},
             {"Keep", GFXS_STENCILOP_KEEP << GFXS1_STENCIL_FRONT_ZFAIL_SHIFT},
         }},
        {"mtlStencilFuncBack",
         1, {
             {"Less", GFXS_STENCILFUNC_LESS << GFXS1_STENCIL_BACK_FUNC_SHIFT},
             {"Equal", GFXS_STENCILFUNC_EQUAL << GFXS1_STENCIL_BACK_FUNC_SHIFT},
             {"LessEqual", GFXS_STENCILFUNC_LESSEQUAL << GFXS1_STENCIL_BACK_FUNC_SHIFT},
             {"Greater", GFXS_STENCILFUNC_GREATER << GFXS1_STENCIL_BACK_FUNC_SHIFT},
             {"NotEqual", GFXS_STENCILFUNC_NOTEQUAL << GFXS1_STENCIL_BACK_FUNC_SHIFT},
             {"GreaterEqual", GFXS_STENCILFUNC_GREATEREQUAL << GFXS1_STENCIL_BACK_FUNC_SHIFT},
             {"Always", GFXS_STENCILFUNC_ALWAYS << GFXS1_STENCIL_BACK_FUNC_SHIFT},
             {"Never", GFXS_STENCILFUNC_NEVER << GFXS1_STENCIL_BACK_FUNC_SHIFT},
         }},
        {"mtlStencilOpBackPass",
         1, {
             {"Zero", GFXS_STENCILOP_ZERO << GFXS1_STENCIL_BACK_PASS_SHIFT},
             {"Replace", GFXS_STENCILOP_REPLACE << GFXS1_STENCIL_BACK_PASS_SHIFT},
             {"IncrSat", GFXS_STENCILOP_INCRSAT << GFXS1_STENCIL_BACK_PASS_SHIFT},
             {"DecrSat", GFXS_STENCILOP_DECRSAT << GFXS1_STENCIL_BACK_PASS_SHIFT},
             {"Invert", GFXS_STENCILOP_INVERT << GFXS1_STENCIL_BACK_PASS_SHIFT},
             {"Incr", GFXS_STENCILOP_INCR << GFXS1_STENCIL_BACK_PASS_SHIFT},
             {"Decr", GFXS_STENCILOP_DECR << GFXS1_STENCIL_BACK_PASS_SHIFT},
             {"Keep", GFXS_STENCILOP_KEEP << GFXS1_STENCIL_BACK_PASS_SHIFT},
         }},
        {"mtlStencilOpBackFail",
         1, {
             {"Zero", GFXS_STENCILOP_ZERO << GFXS1_STENCIL_BACK_FAIL_SHIFT},
             {"Replace", GFXS_STENCILOP_REPLACE << GFXS1_STENCIL_BACK_FAIL_SHIFT},
             {"IncrSat", GFXS_STENCILOP_INCRSAT << GFXS1_STENCIL_BACK_FAIL_SHIFT},
             {"DecrSat", GFXS_STENCILOP_DECRSAT << GFXS1_STENCIL_BACK_FAIL_SHIFT},
             {"Invert", GFXS_STENCILOP_INVERT << GFXS1_STENCIL_BACK_FAIL_SHIFT},
             {"Incr", GFXS_STENCILOP_INCR << GFXS1_STENCIL_BACK_FAIL_SHIFT},
             {"Decr", GFXS_STENCILOP_DECR << GFXS1_STENCIL_BACK_FAIL_SHIFT},
             {"Keep", GFXS_STENCILOP_KEEP << GFXS1_STENCIL_BACK_FAIL_SHIFT},
         }},
        {"mtlStencilOpBackZFail",
         1, {
             {"Zero", GFXS_STENCILOP_ZERO << GFXS1_STENCIL_BACK_ZFAIL_SHIFT},
             {"Replace", GFXS_STENCILOP_REPLACE << GFXS1_STENCIL_BACK_ZFAIL_SHIFT},
             {"IncrSat", GFXS_STENCILOP_INCRSAT << GFXS1_STENCIL_BACK_ZFAIL_SHIFT},
             {"DecrSat", GFXS_STENCILOP_DECRSAT << GFXS1_STENCIL_BACK_ZFAIL_SHIFT},
             {"Invert", GFXS_STENCILOP_INVERT << GFXS1_STENCIL_BACK_ZFAIL_SHIFT},
             {"Incr", GFXS_STENCILOP_INCR << GFXS1_STENCIL_BACK_ZFAIL_SHIFT},
             {"Decr", GFXS_STENCILOP_DECR << GFXS1_STENCIL_BACK_ZFAIL_SHIFT},
             {"Keep", GFXS_STENCILOP_KEEP << GFXS1_STENCIL_BACK_ZFAIL_SHIFT},
         }},
    });

    inline state_map::StateMapLayout stateMapLayout(std::extent_v<decltype(GfxStateBits::loadBits)>, stateMapEntryLayout, stateMapVarLayout);
} // namespace IW4
