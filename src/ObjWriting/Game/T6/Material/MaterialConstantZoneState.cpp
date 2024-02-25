#include "MaterialConstantZoneState.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "Game/T6/GameT6.h"
#include "ObjWriting.h"
#include "Shader/D3D11ShaderAnalyser.h"

#include <chrono>

namespace T6
{
    static constexpr const char* SAMPLER_STR = "Sampler";
    static constexpr const char* GLOBALS_CBUFFER_NAME = "$Globals";
    static constexpr const char* PER_OBJECT_CONSTS_CBUFFER_NAME = "PerObjectConsts";

    const char* KNOWN_CONSTANT_NAMES[]{
        "AngularVelocityScale",
        "AnimSpeed",
        "Background",
        "BackgroundColor",
        "BackgroundNoise",
        "BakedLightingIntensity",
        "BloodBrightness",
        "BloodIntensity",
        "BlurAmount",
        "CapWidth",
        "Char_Size",
        "Char_Width",
        "Coarseness",
        "Color",
        "ColorAmount",
        "ColorBias",
        "Color_Map_Noise",
        "Color_Map_Scale",
        "Color_Map_Size_Scale",
        "DDXScale",
        "DDYScale",
        "DarkenAmount",
        "DarkenPower",
        "Detail_Amount",
        "Detail_Normal_Tile",
        "Diffuse_Normal_Height_Facing",
        "Dimensions",
        "DispersionAmount",
        "Dolly",
        "EdgeColor",
        "EdgeHarshness",
        "EdgeIntensity",
        "EdgeMaxDist",
        "EdgeMinDist",
        "EdgeSize",
        "Edge_Color_Multiplier",
        "Emissive_Amount",
        "EnemiesColor",
        "Exposure",
        "FPS",
        "Fade_Distance",
        "Fill_Direction",
        "Fill_Direction2",
        "FirstFrame",
        "FlareIntensity",
        "FlareScale",
        "FlattenEdges",
        "Flicker_Max",
        "Flicker_Min",
        "Flicker_Seed",
        "Flicker_Speed",
        "Font_Color",
        "Gamma",
        "GlossAmount",
        "Gloss_Amount",
        "Glow_Alt_Color",
        "Glow_Color",
        "Glow_Falloff",
        "GradientColor",
        "GradientMax",
        "GradientMin",
        "Grain_Amount",
        "Grain_Color",
        "Grid",
        "Hardness",
        "Heart_Rate_Offset",
        "Heart_Rate_Scale",
        "Highlight_1_Brightness",
        "Highlight_1_Sharpness",
        "Highlight_2_Brightness",
        "Highlight_2_Sharpness",
        "Highlight_2_Size",
        "Hightlight_1_Size",
        "Holo_Scale",
        "LastFrame",
        "Layer1Alpha",
        "Layer1Depth",
        "Layer1Offset",
        "Layer1OffsetBobbleDelay",
        "Layer1OffsetBobbleSpeedAndSize",
        "Layer1Origin",
        "Layer1Rotation",
        "Layer1Scale",
        "Layer1ScaleBobbleDelay",
        "Layer1ScaleBobbleSpeedAndSize",
        "Layer1Scroll",
        "Layer2Alpha",
        "Layer2Depth",
        "Layer2Offset",
        "Layer2OffsetBobbleDelay",
        "Layer2OffsetBobbleSpeedAndSize",
        "Layer2Origin",
        "Layer2Rotation",
        "Layer2Scale",
        "Layer2ScaleBobbleDelay",
        "Layer2ScaleBobbleSpeedAndSize",
        "Layer2Scroll",
        "Layer3Alpha",
        "Layer3Depth",
        "Layer3Offset",
        "Layer3Origin",
        "Layer3Rotation",
        "Layer3Scale",
        "Layer3Scroll",
        "Layer4Alpha",
        "Layer4Depth",
        "Layer4Offset",
        "Layer4Origin",
        "Layer4Rotation",
        "Layer4Scale",
        "Layer4Scroll",
        "LineColor",
        "LineNoise",
        "LineWidth",
        "MaxDepth",
        "MaxFlickerColor",
        "MaxPulseDepth",
        "MaxResolution",
        "Max_Color",
        "Maximum_Distance",
        "Midlayer_Depth",
        "MinDepth",
        "MinFlickerColor",
        "MinResolution",
        "MinStatic",
        "MinVelocityFraction",
        "Min_Color",
        "Min_Player_Intensity",
        "MomentumColor",
        "NegativeColor",
        "NoisePower",
        "Noise_Scale",
        "NormalHeightMultiplier",
        "Normal_Detail_Height",
        "Normal_Detail_Scale",
        "Normal_Map_Size_Scale",
        "Normal_Variance_Scale",
        "NumFrames",
        "Outline_Lookup_Scale",
        "OverallAmount",
        "OverallBrightness",
        "Overlay_Color",
        "P1",
        "P2",
        "Padding",
        "Player_Color_Multiplier",
        "Player_Lookup_Scale",
        "PositiveColor",
        "Power",
        "PulseColor",
        "PulseInterval",
        "PulseTime",
        "Pulse_Color_Multiplier",
        "Pulse_Lookup_Scale",
        "Radius",
        "ReflectionAmount",
        "Reflection_Amount",
        "Reflection_Blur",
        "Reticle_Alt_Color",
        "Reticle_Color",
        "Row_Chars_",
        "Scale",
        "ScanlineColor",
        "ScanlineIntensity",
        "ScanlineOffset",
        "ScanlinePower",
        "ScanlineSpeed",
        "ScatterAmount",
        "ScatterSize",
        "SceneNoise",
        "SparkleBrightness",
        "SparkleDensity",
        "SparklePower",
        "SparkleProbeAmount",
        "SparkleScale",
        "SparkleSpecAmount",
        "SparkleWash",
        "SpecGloss_Map_Size_Scale",
        "SpecularAmount",
        "SpecularColor",
        "Specular_Amount",
        "Specular_Decay_Threshold",
        "Speed",
        "StaticAmount",
        "StaticLookupSpeed",
        "StaticLookupX",
        "StaticScale",
        "Static_Size",
        "Static_amount",
        "TearLookupMaxX",
        "TearLookupMinX",
        "TearLookupSpeed",
        "TearMultiplier",
        "TearPower",
        "Thickness",
        "TickMarkColorAndHarshness",
        "Tint",
        "VelocityScale",
        "VignetteMultiplier",
        "VignettePower",
        "WarpAmount",
        "WarpHeight",
        "WarpScale",
        "WarpSpeed",
        "WashOut",
        "WashoutMultiply",
        "WaterDirection",
        "WaterHeight",
        "WaterRefraction",
        "WaterScale1",
        "WaterScale2",
        "WaterSpeed1",
        "WaterSpeed2",
        "Zoom",
        "alphaDissolveParms",
        "alphaRevealParms",
        "alphaRevealParms1",
        "alphaRevealParms2",
        "alphaRevealParms3",
        "alphaRevealParms4",
        "clipSpaceLookupOffset",
        "clipSpaceLookupScale",
        "cloudsFeather",
        "cloudsHeights",
        "cloudsUVMad1",
        "cloudsUVMad2",
        "cloudsUVMul1",
        "cloudsUVMul2",
        "codeMeshArg",
        "colorDetailScale",
        "colorObjMax",
        "colorObjMaxBaseBlend",
        "colorObjMin",
        "colorObjMinBaseBlend",
        "colorTint",
        "debugBumpmap",
        "debugPerformance",
        "detailScale",
        "detailScale1",
        "detailScale2",
        "detailScale3",
        "detailScale4",
        "distortionScale",
        "dofEquationScene",
        "dofEquationViewModelAndFarBlur",
        "dofLerpBias",
        "dofLerpDownBias",
        "dofLerpDownScale",
        "dofLerpScale",
        "dofLerpUpBias",
        "dofLerpUpScale",
        "dofRowDelta",
        "eyeOffsetParms",
        "falloffBeginColor",
        "falloffEndColor",
        "falloffParms",
        "featherParms",
        "flagParams",
        "framebufferRead",
        "gameTime",
        "hdrAmount",
        "inverseTransposeWorldMatrix",
        "inverseTransposeWorldViewMatrix",
        "inverseWorldMatrix",
        "inverseWorldViewMatrix",
        "motionblurDirectionAndMagnitude",
        "occlusionAmount",
        "occlusionAmount1",
        "occlusionAmount2",
        "occlusionAmount3",
        "occlusionAmount4",
        "particleCloudColor",
        "particleCloudMatrix",
        "particleCloudVelWorld",
        "resizeParams1",
        "resizeParams2",
        "scaleRGB",
        "scriptVector0",
        "scriptVector1",
        "scriptVector2",
        "scriptVector3",
        "scriptVector4",
        "scriptVector5",
        "scriptVector6",
        "scriptVector7",
        "skyBoxCloudWeights",
        "skyBoxRotationSize",
        "skyColorParms",
        "spotLightWeight",
        "treeCanopyLightingParms",
        "treeCanopyScatterColor",
        "treeCanopySwayParms",
        "ui3dUVSetup0",
        "ui3dUVSetup1",
        "ui3dUVSetup2",
        "ui3dUVSetup3",
        "ui3dUVSetup4",
        "ui3dUVSetup5",
        "uvAnimParms",
        "uvScroll",
        "viewMatrix",
        "weaponParam0",
        "weaponParam1",
        "weaponParam2",
        "weaponParam3",
        "weaponParam4",
        "weaponParam5",
        "weaponParam6",
        "weaponParam7",
        "weaponParam8",
        "weaponParam9",
        "worldViewMatrix",
        "worldViewProjectionMatrix",
    };

    const char* KNOWN_TEXTURE_DEF_NAMES[]{
        "AddMap",
        "Blip_Mask",
        "BlockNoise",
        "CS_Z_buffer",
        "Camo_Detail_Map",
        "Color_Map",
        "CompassMap",
        "Detail_Map",
        "Diffuse",
        "Diffuse_Map",
        "DpadTexture",
        "FontTextutre",
        "Grain_Map",
        "GridTexture",
        "GrimeMap",
        "Heart_Rate_Image",
        "Hologram_Diffuse",
        "Image",
        "Layer1Map",
        "Layer2Map",
        "Layer3Map",
        "Layer4Map",
        "Lookup",
        "Lookup2",
        "LookupMap",
        "Mask",
        "Noise",
        "Noise_Texture",
        "NormalDetailMap",
        "Normal_Detail_Map",
        "Normal_Map",
        "Overlay_Map",
        "Reflection_Mask",
        "Reveal_Map",
        "Rim_Color_Mask",
        "Rim_Specular_Mask",
        "Rim_Occlusion_Mask",
        "Scanline",
        "SparkleMap",
        "SpecularAndGloss",
        "SpecularAndGloss2",
        "Specular_Color_Map",
        "Specular_Gloss_Map",
        "Specular_Map",
        "SpotShadowSamplerState",
        "SpotShadowState",
        "SpriteMap",
        "Static",
        "StaticMap",
        "Static_Noise_Map",
        "SunShadowSamplerState",
        "SunShadowState",
        "Surface_Normal_Map",
        "ThermalMapMask",
        "Thermal_Gradient",
        "Thermal_Map",
        "TickMarkMaterial",
        "Tile",
        "WarpMap",
        "WaterNormalMap",
        "Weapon_Normal_Map",
        "Weapon_Specular_Map",
        "Wireframe",
        "ZBuffer_Map",
        "attenuation",
        "attenuationSampler",
        "baseLut2D",
        "baseLut2DSampler",
        "cinematicA",
        "cinematicASampler",
        "cinematicCb",
        "cinematicCbSampler",
        "cinematicCr",
        "cinematicCrSampler",
        "cinematicY",
        "cinematicYSampler",
        "codeTexture0",
        "codeTexture1",
        "codeTexture2",
        "color",
        "colorDetailMap",
        "colorDetailMapSampler",
        "colorMap",
        "colorMap1",
        "colorMap2",
        "colorMap2D",
        "colorMapPostSun",
        "colorMapPostSunSampler",
        "colorMapSampler",
        "colorMapSampler1",
        "colorMapSampler2",
        "colorSampler",
        "detailMap",
        "detailMapSampler",
        "dlightAttenuation",
        "dlightAttenuationSampler",
        "floatZ",
        "floatZSampler",
        "imageSampler",
        "lightmapSamplerSecondary",
        "lightmapSecondary",
        "lut2D",
        "lut2DSampler",
        "lut3D",
        "lut3DSampler",
        "missileCam",
        "missileCamSampler",
        "modelLighting",
        "modelLightingSampler",
        "normalMap",
        "normalMap1",
        "normalMap2",
        "normalMapSampler",
        "normalMapSampler1",
        "normalMapSampler2",
        "occlusionMap",
        "occlusionMapSampler",
        "occMap",
        "occMapSampler",
        "outdoorMap",
        "outdoorMapSampler",
        "radiantDiffuseMap",
        "rawFloatZ",
        "rawFloatZSampler",
        "reflectionProbe",
        "reflectionProbeSampler",
        "shadowmapSamplerSpot",
        "shadowmapSamplerSun",
        "shadowmapSpot",
        "shadowmapSun",
        "sonarColor",
        "sonarColorSampler",
        "sonarDepth",
        "sonarDepthSampler",
        "source",
        "specularMap",
        "specularMap1",
        "specularMap2",
        "specularMapSampler",
        "specularMapSampler1",
        "specularMapSampler2",
        "stencil",
        "stencilSampler",
        "ui3d",
        "ui3dSampler",
    };

    void MaterialConstantZoneState::ExtractNamesFromZone()
    {
        if (ObjWriting::Configuration.Verbose)
            std::cout << "Building material constant name lookup...\n";

        const auto begin = std::chrono::high_resolution_clock::now();

        AddStaticKnownNames();

        for (const auto* zone : g_GameT6.GetZones())
        {
            const auto* t6AssetPools = dynamic_cast<const GameAssetPoolT6*>(zone->m_pools.get());
            if (!t6AssetPools)
                return;

            for (const auto* techniqueSetInfo : *t6AssetPools->m_technique_set)
            {
                const auto* techniqueSet = techniqueSetInfo->Asset();

                for (const auto* technique : techniqueSet->techniques)
                {
                    if (technique)
                        ExtractNamesFromTechnique(technique);
                }
            }
        }

        const auto end = std::chrono::high_resolution_clock::now();

        if (ObjWriting::Configuration.Verbose)
        {
            const auto durationInMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
            std::cout << "Built material constant name lookup in " << durationInMs.count() << "ms: " << m_constant_names_from_shaders.size()
                      << " constant names; " << m_texture_def_names_from_shaders.size() << " texture def names\n";
        }
    }

    bool MaterialConstantZoneState::GetConstantName(const unsigned hash, std::string& constantName) const
    {
        const auto existingConstantName = m_constant_names_from_shaders.find(hash);
        if (existingConstantName != m_constant_names_from_shaders.end())
        {
            constantName = existingConstantName->second;
            return true;
        }

        return false;
    }

    bool MaterialConstantZoneState::GetTextureDefName(const unsigned hash, std::string& textureDefName) const
    {
        const auto existingTextureDefName = m_texture_def_names_from_shaders.find(hash);
        if (existingTextureDefName != m_texture_def_names_from_shaders.end())
        {
            textureDefName = existingTextureDefName->second;
            return true;
        }

        return false;
    }

    void MaterialConstantZoneState::ExtractNamesFromTechnique(const MaterialTechnique* technique)
    {
        const auto existingTechnique = m_dumped_techniques.find(technique);
        if (existingTechnique != m_dumped_techniques.end())
            return;

        m_dumped_techniques.emplace(technique);

        for (auto passIndex = 0u; passIndex < technique->passCount; passIndex++)
        {
            const auto& pass = technique->passArray[passIndex];

            if (pass.vertexShader && pass.vertexShader->prog.loadDef.program)
                ExtractNamesFromShader(pass.vertexShader->prog.loadDef.program, pass.vertexShader->prog.loadDef.programSize);

            if (pass.pixelShader && pass.pixelShader->prog.loadDef.program)
                ExtractNamesFromShader(pass.pixelShader->prog.loadDef.program, pass.pixelShader->prog.loadDef.programSize);
        }
    }

    void MaterialConstantZoneState::ExtractNamesFromShader(const char* shader, const size_t shaderSize)
    {
        const auto shaderInfo = d3d11::ShaderAnalyser::GetShaderInfo(reinterpret_cast<const uint8_t*>(shader), shaderSize);
        if (!shaderInfo)
            return;

        const auto globalsConstantBuffer = std::ranges::find_if(std::as_const(shaderInfo->m_constant_buffers),
                                                                [](const d3d11::ConstantBuffer& constantBuffer)
                                                                {
                                                                    return constantBuffer.m_name == GLOBALS_CBUFFER_NAME;
                                                                });

        const auto perObjectConsts = std::ranges::find_if(std::as_const(shaderInfo->m_constant_buffers),
                                                          [](const d3d11::ConstantBuffer& constantBuffer)
                                                          {
                                                              return constantBuffer.m_name == PER_OBJECT_CONSTS_CBUFFER_NAME;
                                                          });

        if (globalsConstantBuffer != shaderInfo->m_constant_buffers.end())
        {
            for (const auto& variable : globalsConstantBuffer->m_variables)
                AddConstantName(variable.m_name);
        }

        if (perObjectConsts != shaderInfo->m_constant_buffers.end())
        {
            for (const auto& variable : perObjectConsts->m_variables)
                AddConstantName(variable.m_name);
        }

        for (const auto& boundResource : shaderInfo->m_bound_resources)
        {
            if (boundResource.m_type == d3d11::BoundResourceType::SAMPLER || boundResource.m_type == d3d11::BoundResourceType::TEXTURE)
            {
                if (AddTextureDefName(boundResource.m_name))
                {
                    const auto samplerPos = boundResource.m_name.rfind(SAMPLER_STR);
                    if (samplerPos != std::string::npos)
                    {
                        auto nameWithoutSamplerStr = boundResource.m_name;
                        nameWithoutSamplerStr.erase(samplerPos, std::char_traits<char>::length(SAMPLER_STR));
                        AddTextureDefName(std::move(nameWithoutSamplerStr));
                    }
                }
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

    void MaterialConstantZoneState::AddConstantName(std::string constantName)
    {
        const auto hash = Common::R_HashString(constantName.c_str(), 0);
        if (m_constant_names_from_shaders.contains(hash))
            return;

        m_constant_names_from_shaders.emplace(hash, std::move(constantName));
    }

    bool MaterialConstantZoneState::AddTextureDefName(std::string textureDefName)
    {
        const auto hash = Common::R_HashString(textureDefName.c_str(), 0);
        if (m_texture_def_names_from_shaders.contains(hash))
            return false;

        m_texture_def_names_from_shaders.emplace(hash, std::move(textureDefName));
        return true;
    }
} // namespace T6
