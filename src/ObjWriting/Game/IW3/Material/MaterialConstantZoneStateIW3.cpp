#include "MaterialConstantZoneStateIW3.h"

#include "Game/IW3/CommonIW3.h"
#include "Game/IW3/GameAssetPoolIW3.h"
#include "Game/IW3/GameIW3.h"
#include "ObjWriting.h"

namespace IW3
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

    void MaterialConstantZoneState::ExtractNamesFromZoneInternal()
    {
        for (const auto* zone : IGame::GetGameById(GameId::IW3)->GetZones())
        {
            const auto* assetPools = dynamic_cast<const GameAssetPoolIW3*>(zone->m_pools.get());
            if (!assetPools)
                return;

            for (const auto* techniqueSetInfo : *assetPools->m_technique_set)
            {
                const auto* techniqueSet = techniqueSetInfo->Asset();

                for (const auto* technique : techniqueSet->techniques)
                {
                    if (technique)
                        ExtractNamesFromTechnique(technique);
                }
            }
        }
    }

    unsigned MaterialConstantZoneState::HashString(const std::string& str)
    {
        return Common::R_HashString(str.c_str());
    }

    void MaterialConstantZoneState::ExtractNamesFromTechnique(const MaterialTechnique* technique)
    {
        if (!ShouldDumpFromStruct(technique))
            return;

        for (auto passIndex = 0u; passIndex < technique->passCount; passIndex++)
        {
            const auto& pass = technique->passArray[passIndex];

            if (pass.vertexShader && pass.vertexShader->prog.loadDef.program)
                ExtractNamesFromShader(pass.vertexShader->prog.loadDef.program, pass.vertexShader->prog.loadDef.programSize);

            if (pass.pixelShader && pass.pixelShader->prog.loadDef.program)
                ExtractNamesFromShader(pass.pixelShader->prog.loadDef.program, pass.pixelShader->prog.loadDef.programSize);
        }
    }

    void MaterialConstantZoneState::AddStaticKnownNames()
    {
        for (const auto* knownConstantName : KNOWN_CONSTANT_NAMES)
            AddConstantName(knownConstantName);
        for (const auto* knownTextureDefName : KNOWN_TEXTURE_DEF_NAMES)
            AddTextureDefName(knownTextureDefName);
    }
} // namespace IW3
