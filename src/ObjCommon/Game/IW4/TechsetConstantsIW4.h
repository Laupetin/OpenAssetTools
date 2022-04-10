#pragma once

#include <unordered_map>
#include <type_traits>

#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    inline const char* techniqueTypeNames[]
    {
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

    static const char* materialStreamDestinationNames[]
    {
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

    static const char* materialStreamSourceNames[]
    {
        "position",
        "color",
        "texcoord[0]",
        "normal",
        "tangent",
        "texcoord[1]",
        "texcoord[2]",
        "normalTransform[0]",
        "normalTransform[1]"
    };
    static_assert(std::extent_v<decltype(materialStreamSourceNames)> == STREAM_SRC_COUNT);

    inline CodeSamplerSource s_lightmapSamplers[]
    {
        {"primary", TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY, nullptr, 0, 0},
        {"secondary", TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY, nullptr, 0, 0},
        {}
    };

    inline CodeSamplerSource s_lightSamplers[]
    {
        {"attenuation", TEXTURE_SRC_CODE_LIGHT_ATTENUATION, nullptr, 0, 0},
        {}
    };

    inline CodeSamplerSource s_codeSamplers[]
    {
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
        {}
    };

    inline CodeSamplerSource s_defaultCodeSamplers[]
    {
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
        {}
    };

    inline CodeConstantSource s_sunConsts[]
    {
        {"position", CONST_SRC_CODE_LIGHT_POSITION, nullptr, 0, 0},
        {"diffuse", CONST_SRC_CODE_LIGHT_DIFFUSE, nullptr, 0, 0},
        {"specular", CONST_SRC_CODE_LIGHT_SPECULAR, nullptr, 0, 0},
        {"spotDir", CONST_SRC_CODE_LIGHT_SPOTDIR, nullptr, 0, 0},
        {"spotFactors", CONST_SRC_CODE_LIGHT_SPOTFACTORS, nullptr, 0, 0},
        {"falloffPlacement", CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT, nullptr, 0, 0},
        {}
    };

    inline CodeConstantSource s_nearPlaneConsts[]
    {
        {"org", CONST_SRC_CODE_NEARPLANE_ORG, nullptr, 0, 0},
        {"dx", CONST_SRC_CODE_NEARPLANE_DX, nullptr, 0, 0},
        {"dy", CONST_SRC_CODE_NEARPLANE_DY, nullptr, 0, 0},
        {}
    };

    inline CodeConstantSource s_codeConsts[]
    {
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
        {}
    };

    inline CodeConstantSource s_defaultCodeConsts[]
    {
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
        {}
    };

    static constexpr std::pair<uint32_t, const char*> KnownMaterialSource(const char* name)
    {
        return std::make_pair(Common::R_HashString(name, 0u), name);
    }

    inline std::unordered_map knownMaterialSourceNames
    {
        KnownMaterialSource("colorMap"),
        KnownMaterialSource("colorMap0"),
        KnownMaterialSource("colorMap1"),
        KnownMaterialSource("colorMap2"),
        KnownMaterialSource("colorMap3"),
        KnownMaterialSource("colorMap4"),
        KnownMaterialSource("colorMap5"),
        KnownMaterialSource("colorMap6"),
        KnownMaterialSource("colorMap7"),
        KnownMaterialSource("normalMap"),
        KnownMaterialSource("normalMap0"),
        KnownMaterialSource("normalMap1"),
        KnownMaterialSource("normalMap2"),
        KnownMaterialSource("normalMap3"),
        KnownMaterialSource("normalMap4"),
        KnownMaterialSource("normalMap5"),
        KnownMaterialSource("normalMap6"),
        KnownMaterialSource("normalMap7"),
        KnownMaterialSource("specularMap"),
        KnownMaterialSource("specularMap0"),
        KnownMaterialSource("specularMap1"),
        KnownMaterialSource("specularMap2"),
        KnownMaterialSource("specularMap3"),
        KnownMaterialSource("specularMap4"),
        KnownMaterialSource("specularMap5"),
        KnownMaterialSource("specularMap6"),
        KnownMaterialSource("specularMap7"),
        KnownMaterialSource("detailMap"),
        KnownMaterialSource("detailMap0"),
        KnownMaterialSource("detailMap1"),
        KnownMaterialSource("detailMap2"),
        KnownMaterialSource("detailMap3"),
        KnownMaterialSource("detailMap4"),
        KnownMaterialSource("detailMap5"),
        KnownMaterialSource("detailMap6"),
        KnownMaterialSource("detailMap7"),
        KnownMaterialSource("attenuationMap"),
        KnownMaterialSource("attenuationMap0"),
        KnownMaterialSource("attenuationMap1"),
        KnownMaterialSource("attenuationMap2"),
        KnownMaterialSource("attenuationMap3"),
        KnownMaterialSource("attenuationMap4"),
        KnownMaterialSource("attenuationMap5"),
        KnownMaterialSource("attenuationMap6"),
        KnownMaterialSource("attenuationMap7"),
    };
}
