#include "MaterialConstantZoneStateIW5.h"

#include "Game/IW5/CommonIW5.h"
#include "Game/IW5/GameIW5.h"
#include "ObjWriting.h"
#include "Zone/ZoneRegistry.h"

namespace
{
    const char* KNOWN_CONSTANT_NAMES[]{
        "worldViewProjectionMatrix",
        "worldViewMatrix2",
        "worldViewMatrix1",
        "worldViewMatrix",
        "worldOutdoorLookupMatrix",
        "worldMatrix",
        "waterColor",
        "viewportDimensions",
        "viewProjectionMatrix",
        "uvScale",
        "uvAnimParms",
        "thermalColorOffset",
        "sunShadowmapPixelAdjust",
        "ssaoParms",
        "spotShadowmapPixelAdjust",
        "shadowmapSwitchPartition",
        "shadowmapScale",
        "shadowmapPolygonOffset",
        "shadowLookupMatrix",
        "renderTargetSize",
        "renderSourceSize",
        "projectionMatrix",
        "playlistPopulationParams",
        "pixelCostFracs",
        "pixelCostDecode",
        "particleCloudSparkColor2",
        "particleCloudSparkColor1",
        "particleCloudSparkColor0",
        "particleCloudMatrix2",
        "particleCloudMatrix1",
        "particleCloudMatrix",
        "particleCloudColor",
        "outdoorFeatherParms",
        "oceanUVAnimParmPaintedFoam",
        "oceanUVAnimParmOctave2",
        "oceanUVAnimParmOctave1",
        "oceanUVAnimParmOctave0",
        "oceanUVAnimParmFoam",
        "oceanUVAnimParmDetail1",
        "oceanUVAnimParmDetail0",
        "oceanScrollParms",
        "oceanMiscParms",
        "oceanFoamParms",
        "oceanAmplitude",
        "materialColor",
        "lightprobeAmbient",
        "lightingLookupScale",
        "lightSpotFactors",
        "lightSpotDir",
        "lightSpecular",
        "lightPosition",
        "lightFalloffPlacement",
        "lightDiffuse",
        "inverseWorldViewMatrix",
        "inverseViewProjectionMatrix",
        "inverseTransposeWorldViewMatrix",
        "heatMapDetail",
        "glowSetup",
        "glowApply",
        "gameTime",
        "fullscreenDistortion",
        "fogSunDir",
        "fogSunConsts",
        "fogSunColorLinear",
        "fogSunColorGamma",
        "fogConsts",
        "fogColorLinear",
        "fogColorGamma",
        "flagParms",
        "filterTap",
        "featherParms",
        "falloffParms",
        "falloffEndColor",
        "falloffBeginColor",
        "fadeEffect",
        "eyeOffsetParms",
        "eyeOffset",
        "envMapParms",
        "dustTint",
        "dustParms",
        "dustEyeParms",
        "dofRowDelta",
        "dofLerpScale",
        "dofLerpBias",
        "dofEquationViewModelAndFarBlur",
        "dofEquationScene",
        "distortionScale",
        "detailScale",
        "depthFromClip",
        "debugBumpmap",
        "colorTintQuadraticDelta",
        "colorTintDelta",
        "colorTintBase",
        "colorSaturationR",
        "colorSaturationG",
        "colorSaturationB",
        "colorObjMin",
        "colorObjMax",
        "colorMatrixR",
        "colorMatrixG",
        "colorMatrixB",
        "colorBias",
        "codeMeshArg",
        "clipSpaceLookupScale",
        "clipSpaceLookupOffset",
        "baseLightingCoords",
    };

    const char* KNOWN_TEXTURE_DEF_NAMES[]{
        "attenuation",
        "attenuationSampler",
        "cinematicA",
        "cinematicASampler",
        "cinematicCb",
        "cinematicCbSampler",
        "cinematicCr",
        "cinematicCrSampler",
        "cinematicY",
        "cinematicYSampler",
        "colorMap",
        "colorMap1",
        "colorMap2",
        "colorMapPostSun",
        "colorMapPostSunSampler",
        "colorMapSampler",
        "colorMapSampler1",
        "colorMapSampler2",
        "cucoloris",
        "cucolorisSampler",
        "detailMap",
        "detailMapSampler",
        "dust",
        "dustSampler",
        "fadeMap",
        "fadeMapSampler",
        "floatZ",
        "floatZSampler",
        "grainMap",
        "grainMapSampler",
        "halfParticleColor",
        "halfParticleColorSampler",
        "halfParticleDepth",
        "halfParticleDepthSampler",
        "heatmap",
        "heatmapSampler",
        "lightmapPrimary",
        "lightmapSamplerPrimary",
        "lightmapSamplerSecondary",
        "lightmapSecondary",
        "lookupMap",
        "lookupMapSampler",
        "modelLighting",
        "modelLightingSampler",
        "normalMap",
        "normalMapSampler",
        "oceanColorRamp",
        "oceanColorRampSampler",
        "oceanDetailNormal",
        "oceanDetailNormalSampler",
        "oceanDisplacement",
        "oceanDisplacementSampler",
        "oceanEnv",
        "oceanEnvSampler",
        "oceanFoam",
        "oceanFoamSampler",
        "oceanHeightNormal",
        "oceanHeightNormalSampler",
        "oceanPaintedFoam",
        "oceanPaintedFoamSampler",
        "outdoorMap",
        "outdoorMapSampler",
        "population",
        "populationSampler",
        "reflectionProbe",
        "reflectionProbeSampler",
        "shadowmapSamplerSpot",
        "shadowmapSamplerSun",
        "shadowmapSpot",
        "shadowmapSun",
        "skyMap",
        "skyMapSampler",
        "specularMap",
        "specularMapSampler",
        "ssao",
        "ssaoSampler",
        "worldMap",
        "worldMapSampler",
    };
} // namespace

namespace IW5
{
    void MaterialConstantZoneState::ExtractNamesFromZoneInternal()
    {
        for (const auto* zone : ZoneRegistry::GetRegistryForGame(GameId::IW5)->Zones())
        {
            for (const auto* vertexShaderAsset : zone->m_pools.PoolAssets<AssetVertexShader>())
            {
                const auto* vertexShader = vertexShaderAsset->Asset();
                if (ShouldDumpFromStruct(vertexShader))
                    ExtractNamesFromShader(vertexShader->prog.loadDef.program, static_cast<size_t>(vertexShader->prog.loadDef.programSize) * sizeof(uint32_t));
            }

            for (const auto* pixelShaderAsset : zone->m_pools.PoolAssets<AssetPixelShader>())
            {
                const auto* pixelShader = pixelShaderAsset->Asset();
                if (ShouldDumpFromStruct(pixelShader))
                    ExtractNamesFromShader(pixelShader->prog.loadDef.program, static_cast<size_t>(pixelShader->prog.loadDef.programSize) * sizeof(uint32_t));
            }
        }
    }

    void MaterialConstantZoneState::AddStaticKnownNames()
    {
        for (const auto* knownConstantName : KNOWN_CONSTANT_NAMES)
            AddConstantName(knownConstantName);
        for (const auto* knownTextureDefName : KNOWN_TEXTURE_DEF_NAMES)
            AddTextureDefName(knownTextureDefName);
    }

    unsigned MaterialConstantZoneState::HashString(const std::string& str) const
    {
        return Common::R_HashString(str.c_str());
    }
} // namespace IW5
