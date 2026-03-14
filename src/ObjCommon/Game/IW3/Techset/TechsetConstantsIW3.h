#pragma once

#include "Game/IW3/IW3.h"
#include "Techset/CommonTechnique.h"
#include "Techset/CommonTechset.h"

#include <type_traits>

namespace IW3
{
    inline const char* techniqueTypeNames[]{
        "depth prepass",
        "build floatz",
        "build shadowmap depth",
        "build shadowmap color",
        "unlit",
        "emissive",
        "emissive shadow",
        "lit",
        "lit sun",
        "lit sun shadow",
        "lit spot",
        "lit spot shadow",
        "lit omni",
        "lit omni shadow",
        "lit instanced",
        "lit instanced sun",
        "lit instanced sun shadow",
        "lit instanced spot",
        "lit instanced spot shadow",
        "lit instanced omni",
        "lit instanced omni shadow",
        "light spot",
        "light omni",
        "light spot shadow",
        "fakelight normal",
        "fakelight view",
        "sunlight preview",
        "case texture",
        "solid wireframe",
        "shaded wireframe",
        "shadowcookie caster",
        "shadowcookie receiver",
        "debug bumpmap",
        "debug bumpmap instanced",
    };
    static_assert(std::extent_v<decltype(techniqueTypeNames)> == TECHNIQUE_COUNT);
    static inline techset::CommonTechniqueTypeNames commonTechniqueTypeNames(techniqueTypeNames, std::extent_v<decltype(techniqueTypeNames)>);

    static inline techset::CommonStreamRoutingSourceInfo streamRoutingSources[]{
        {
         .name = "position",
         .abbreviation = "p",
         .optional = false,
         },
        {
         .name = "color",
         .abbreviation = "c",
         .optional = false,
         },
        {
         .name = "texcoord[0]",
         .abbreviation = "t0",
         .optional = false,
         },
        {
         .name = "normal",
         .abbreviation = "n",
         .optional = false,
         },
        {
         .name = "tangent",
         .abbreviation = "t",
         .optional = false,
         },
        {
         .name = "texcoord[1]",
         .abbreviation = "t1",
         .optional = true,
         },
        {
         .name = "texcoord[2]",
         .abbreviation = "t2",
         .optional = true,
         },
        {
         .name = "normalTransform[0]",
         .abbreviation = "n0",
         .optional = true,
         },
        {
         .name = "normalTransform[1]",
         .abbreviation = "n1",
         .optional = true,
         },
    };
    static_assert(std::extent_v<decltype(streamRoutingSources)> == STREAM_SRC_COUNT);

    static inline techset::CommonStreamRoutingDestinationInfo streamRoutingDestinations[]{
        {
         .name = "position",
         .abbreviation = "p",
         },
        {
         .name = "normal",
         .abbreviation = "n",
         },
        {
         .name = "color[0]",
         .abbreviation = "c0",
         },
        {
         .name = "color[1]",
         .abbreviation = "c1",
         },
        {
         .name = "texcoord[0]",
         .abbreviation = "t0",
         },
        {
         .name = "texcoord[1]",
         .abbreviation = "t1",
         },
        {
         .name = "texcoord[2]",
         .abbreviation = "t2",
         },
        {
         .name = "texcoord[3]",
         .abbreviation = "t3",
         },
        {
         .name = "texcoord[4]",
         .abbreviation = "t4",
         },
        {
         .name = "texcoord[5]",
         .abbreviation = "t5",
         },
        {
         .name = "texcoord[6]",
         .abbreviation = "t6",
         },
        {
         .name = "texcoord[7]",
         .abbreviation = "t7",
         },
    };
    static_assert(std::extent_v<decltype(streamRoutingDestinations)> == STREAM_DST_COUNT);

    static inline techset::CommonStreamRoutingInfos commonRoutingInfos(streamRoutingSources,
                                                                       std::extent_v<decltype(streamRoutingSources)>,
                                                                       streamRoutingDestinations,
                                                                       std::extent_v<decltype(streamRoutingDestinations)>);

    static inline techset::CommonCodeConstSourceInfo commonCodeConstSources[]{
        {
         .value = CONST_SRC_CODE_LIGHT_POSITION,
         .accessor = "lightPosition",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_LIGHT_DIFFUSE,
         .accessor = "lightDiffuse",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_LIGHT_SPECULAR,
         .accessor = "lightSpecular",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_LIGHT_SPOTDIR,
         .accessor = "lightSpotDir",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_LIGHT_SPOTFACTORS,
         .accessor = "lightSpotFactors",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         .techFlags = MTL_TECHFLAG_USES_LIGHT_SPOT_FACTORS,
         },
        {
         .value = CONST_SRC_CODE_NEARPLANE_ORG,
         .accessor = "nearPlaneOrg",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_NEARPLANE_DX,
         .accessor = "nearPlaneDx",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_NEARPLANE_DY,
         .accessor = "nearPlaneDy",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_SHADOW_PARMS,
         .accessor = "shadowParms",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET,
         .accessor = "shadowmapPolygonOffset",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_RENDER_TARGET_SIZE,
         .accessor = "renderTargetSize",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT,
         .accessor = "lightFalloffPlacement",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR,
         .accessor = "dofEquationViewModelAndFarBlur",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_DOF_EQUATION_SCENE,
         .accessor = "dofEquationScene",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_DOF_LERP_SCALE,
         .accessor = "dofLerpScale",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_DOF_LERP_BIAS,
         .accessor = "dofLerpBias",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_DOF_ROW_DELTA,
         .accessor = "dofRowDelta",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_PARTICLE_CLOUD_COLOR,
         .accessor = "particleCloudColor",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_GAMETIME,
         .accessor = "gameTime",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_PIXEL_COST_FRACS,
         .accessor = "pixelCostFracs",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_PIXEL_COST_DECODE,
         .accessor = "pixelCostDecode",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_FILTER_TAP_0,
         .accessor = "filterTap",
         .arrayCount = 8,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_COLOR_MATRIX_R,
         .accessor = "colorMatrixR",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_COLOR_MATRIX_G,
         .accessor = "colorMatrixG",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_COLOR_MATRIX_B,
         .accessor = "colorMatrixB",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION,
         .accessor = "shadowmapSwitchPartition",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_SHADOWMAP_SCALE,
         .accessor = "shadowmapScale",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_ZNEAR,
         .accessor = "zNear",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_SUN_POSITION,
         .accessor = "sunPosition",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_SUN_DIFFUSE,
         .accessor = "sunDiffuse",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_SUN_SPECULAR,
         .accessor = "sunSpecular",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE,
         .accessor = "lightingLookupScale",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_DEBUG_BUMPMAP,
         .accessor = "debugBumpmap",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_MATERIAL_COLOR,
         .accessor = "materialColor",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_FOG,
         .accessor = "fogConsts",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_FOG_COLOR,
         .accessor = "fogColor",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_GLOW_SETUP,
         .accessor = "glowSetup",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_GLOW_APPLY,
         .accessor = "glowApply",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_COLOR_BIAS,
         .accessor = "colorBias",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_COLOR_TINT_BASE,
         .accessor = "colorTintBase",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_COLOR_TINT_DELTA,
         .accessor = "colorTintDelta",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS,
         .accessor = "outdoorFeatherParms",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_ENVMAP_PARMS,
         .accessor = "envMapParms",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST,
         .accessor = "spotShadowmapPixelAdjust",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE,
         .accessor = "clipSpaceLookupScale",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET,
         .accessor = "clipSpaceLookupOffset",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX,
         .accessor = "particleCloudMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = CONST_SRC_CODE_DEPTH_FROM_CLIP,
         .accessor = "depthFromClip",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = CONST_SRC_CODE_CODE_MESH_ARG_0,
         .accessor = "codeMeshArg",
         .arrayCount = 2,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = CONST_SRC_CODE_BASE_LIGHTING_COORDS,
         .accessor = "baseLightingCoords",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         },
        {
         .value = CONST_SRC_CODE_WORLD_MATRIX,
         .accessor = "worldMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_WORLD_MATRIX,
         .accessor = "inverseWorldMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX,
         .accessor = "transposeWorldMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_WORLD_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX,
         .accessor = "inverseTransposeWorldMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_WORLD_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_VIEW_MATRIX,
         .accessor = "viewMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_VIEW_MATRIX,
         .accessor = "inverseViewMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX,
         .accessor = "transposeViewMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_VIEW_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX,
         .accessor = "inverseTransposeViewMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_VIEW_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_PROJECTION_MATRIX,
         .accessor = "projectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX,
         .accessor = "inverseProjectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX,
         .accessor = "transposeProjectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX,
         .accessor = "inverseTransposeProjectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_WORLD_VIEW_MATRIX,
         .accessor = "worldViewMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX,
         .accessor = "inverseWorldViewMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX,
         .accessor = "transposeWorldViewMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_WORLD_VIEW_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX,
         .accessor = "inverseTransposeWorldViewMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_VIEW_PROJECTION_MATRIX,
         .accessor = "viewProjectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX,
         .accessor = "inverseViewProjectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX,
         .accessor = "transposeViewProjectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_VIEW_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX,
         .accessor = "inverseTransposeViewProjectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX,
         .accessor = "worldViewProjectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX,
         .accessor = "inverseWorldViewProjectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX,
         .accessor = "transposeWorldViewProjectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX,
         .accessor = "inverseTransposeWorldViewProjectionMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX,
         .accessor = "shadowLookupMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX,
         .accessor = "inverseShadowLookupMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX,
         .accessor = "transposeShadowLookupMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX,
         .accessor = "inverseTransposeShadowLookupMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX,
         .accessor = "worldOutdoorLookupMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
         .accessor = "inverseWorldOutdoorLookupMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
         .accessor = "transposeWorldOutdoorLookupMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX,
         },
        {
         .value = CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
         .accessor = "inverseTransposeWorldOutdoorLookupMatrix",
         .arrayCount = 0,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_PRIM,
         .transposedMatrix = CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
         },
    };

    static inline techset::CommonCodeSamplerSourceInfo commonCodeSamplerSources[]{
        {
         .value = TEXTURE_SRC_CODE_BLACK,
         .accessor = "black",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_WHITE,
         .accessor = "white",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP,
         .accessor = "identityNormalMap",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_MODEL_LIGHTING,
         .accessor = "modelLightingSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY,
         .accessor = "lightmapSamplerPrimary",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::CUSTOM,
         .customSamplerIndex = CUSTOM_SAMPLER_LIGHTMAP_PRIMARY,
         },
        {
         .value = TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY,
         .accessor = "lightmapSamplerSecondary",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::CUSTOM,
         .customSamplerIndex = CUSTOM_SAMPLER_LIGHTMAP_SECONDARY,
         },
        {
         .value = TEXTURE_SRC_CODE_SHADOWCOOKIE,
         .accessor = "shadowCookieSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_SHADOWMAP_SUN,
         .accessor = "shadowmapSamplerSun",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_SHADOWMAP_SPOT,
         .accessor = "shadowmapSamplerSpot",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_FEEDBACK,
         .accessor = "feedbackSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = TEXTURE_SRC_CODE_RESOLVED_POST_SUN,
         .accessor = "resolvedPostSun",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_RESOLVED_SCENE,
         .accessor = "resolvedScene",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_POST_EFFECT_0,
         .accessor = "postEffect0",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_POST_EFFECT_1,
         .accessor = "postEffect1",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_SKY,
         .accessor = "sky",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_LIGHT_ATTENUATION,
         .accessor = "attenuationSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = TEXTURE_SRC_CODE_DYNAMIC_SHADOWS,
         .accessor = "dynamicShadowSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = TEXTURE_SRC_CODE_OUTDOOR,
         .accessor = "outdoor",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         },
        {
         .value = TEXTURE_SRC_CODE_FLOATZ,
         .accessor = "floatZSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         .techFlags = MTL_TECHFLAG_USES_FLOATZ,
         },
        {
         .value = TEXTURE_SRC_CODE_PROCESSED_FLOATZ,
         .accessor = "processedFloatZSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         .techFlags = MTL_TECHFLAG_USES_FLOATZ,
         },
        {
         .value = TEXTURE_SRC_CODE_RAW_FLOATZ,
         .accessor = "rawFloatZSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::RARELY,
         .techFlags = MTL_TECHFLAG_USES_FLOATZ,
         },
        {
         .value = TEXTURE_SRC_CODE_CASE_TEXTURE,
         .accessor = "caseTexture" /* ? */,
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = TEXTURE_SRC_CODE_CINEMATIC_Y,
         .accessor = "cinematicYSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = TEXTURE_SRC_CODE_CINEMATIC_CR,
         .accessor = "cinematicCrSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = TEXTURE_SRC_CODE_CINEMATIC_CB,
         .accessor = "cinematicCbSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = TEXTURE_SRC_CODE_CINEMATIC_A,
         .accessor = "cinematicASampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::PER_OBJECT,
         },
        {
         .value = TEXTURE_SRC_CODE_REFLECTION_PROBE,
         .accessor = "reflectionProbeSampler",
         .updateFrequency = techset::CommonCodeSourceUpdateFrequency::CUSTOM,
         .customSamplerIndex = CUSTOM_SAMPLER_REFLECTION_PROBE,
         },
    };

    // See MaterialShaderArgumentType
    static inline techset::CommonShaderArgumentType commonArgumentTypes[]{
        {.m_shader_type = techset::CommonTechniqueShaderType::VERTEX, .m_value_type = techset::CommonShaderValueType::MATERIAL_CONST  },
        {.m_shader_type = techset::CommonTechniqueShaderType::VERTEX, .m_value_type = techset::CommonShaderValueType::LITERAL_CONST   },
        {.m_shader_type = techset::CommonTechniqueShaderType::PIXEL,  .m_value_type = techset::CommonShaderValueType::MATERIAL_SAMPLER},
        {.m_shader_type = techset::CommonTechniqueShaderType::VERTEX, .m_value_type = techset::CommonShaderValueType::CODE_CONST      },
        {.m_shader_type = techset::CommonTechniqueShaderType::PIXEL,  .m_value_type = techset::CommonShaderValueType::CODE_SAMPLER    },
        {.m_shader_type = techset::CommonTechniqueShaderType::PIXEL,  .m_value_type = techset::CommonShaderValueType::CODE_CONST      },
        {.m_shader_type = techset::CommonTechniqueShaderType::PIXEL,  .m_value_type = techset::CommonShaderValueType::MATERIAL_CONST  },
        {.m_shader_type = techset::CommonTechniqueShaderType::PIXEL,  .m_value_type = techset::CommonShaderValueType::LITERAL_CONST   },
    };
    static_assert(std::extent_v<decltype(commonArgumentTypes)> == MTL_ARG_COUNT);

    static inline techset::CommonCodeSourceInfos commonCodeSourceInfos(commonCodeConstSources,
                                                                       std::extent_v<decltype(commonCodeConstSources)>,
                                                                       commonCodeSamplerSources,
                                                                       std::extent_v<decltype(commonCodeSamplerSources)>,
                                                                       nullptr,
                                                                       0,
                                                                       commonArgumentTypes,
                                                                       std::extent_v<decltype(commonArgumentTypes)>);

    static inline MaterialTypeInfo g_materialTypeInfo[]{
        {"",    ""   },
        {"m/",  "m_" },
        {"mc/", "mc_"},
        {"w/",  "w_" },
        {"wc/", "wc_"},
    };
    static_assert(std::extent_v<decltype(g_materialTypeInfo)> == MTL_TYPE_COUNT);
} // namespace IW3
