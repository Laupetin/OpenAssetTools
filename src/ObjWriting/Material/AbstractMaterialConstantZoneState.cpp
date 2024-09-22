#include "AbstractMaterialConstantZoneState.h"

#include "ObjWriting.h"
#include "Shader/D3D11ShaderAnalyser.h"
#include "Shader/D3D9ShaderAnalyser.h"

#include <chrono>

namespace
{
    constexpr const char* SAMPLER_STR = "Sampler";
    constexpr const char* GLOBALS_CBUFFER_NAME = "$Globals";
    constexpr const char* PER_OBJECT_CONSTS_CBUFFER_NAME = "PerObjectConsts";
} // namespace

void AbstractMaterialConstantZoneState::ExtractNamesFromZone()
{
    if (ObjWriting::Configuration.Verbose)
        std::cout << "Building material constant name lookup...\n";

    const auto begin = std::chrono::high_resolution_clock::now();

    AddStaticKnownNames();

    ExtractNamesFromZoneInternal();

    const auto end = std::chrono::high_resolution_clock::now();

    if (ObjWriting::Configuration.Verbose)
    {
        const auto durationInMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        std::cout << std::format("Built material constant name lookup in {}ms: {} constant names; {} texture def names\n",
                                 durationInMs.count(),
                                 m_constant_names_from_shaders.size(),
                                 m_texture_def_names_from_shaders.size());
    }
}

bool AbstractMaterialConstantZoneState::GetConstantName(const unsigned hash, std::string& constantName) const
{
    const auto existingConstantName = m_constant_names_from_shaders.find(hash);
    if (existingConstantName != m_constant_names_from_shaders.end())
    {
        constantName = existingConstantName->second;
        return true;
    }

    return false;
}

bool AbstractMaterialConstantZoneState::GetTextureDefName(const unsigned hash, std::string& textureDefName) const
{
    const auto existingTextureDefName = m_texture_def_names_from_shaders.find(hash);
    if (existingTextureDefName != m_texture_def_names_from_shaders.end())
    {
        textureDefName = existingTextureDefName->second;
        return true;
    }

    return false;
}

bool AbstractMaterialConstantZoneState::ShouldDumpFromStruct(const void* pStruct)
{
    const auto existingTextureDefName = m_dumped_structs.find(pStruct);
    if (existingTextureDefName != m_dumped_structs.end())
        return false;

    m_dumped_structs.emplace(pStruct);
    return true;
}

void AbstractMaterialConstantZoneState::AddConstantName(const std::string& constantName)
{
    const auto hash = HashString(constantName);
    if (m_constant_names_from_shaders.contains(hash))
        return;

    m_constant_names_from_shaders.emplace(hash, constantName);
}

bool AbstractMaterialConstantZoneState::AddTextureDefName(const std::string& textureDefName)
{
    const auto hash = HashString(textureDefName);
    if (m_texture_def_names_from_shaders.contains(hash))
        return false;

    m_texture_def_names_from_shaders.emplace(hash, textureDefName);
    return true;
}

void AbstractMaterialConstantZoneStateDx9::ExtractNamesFromShader(const void* shader, const size_t shaderSize)
{
    const auto shaderInfo = d3d9::ShaderAnalyser::GetShaderInfo(shader, shaderSize);
    if (!shaderInfo)
        return;

    for (const auto& constant : shaderInfo->m_constants)
    {
        if (constant.m_register_set == d3d9::RegisterSet::SAMPLER)
        {
            if (AddTextureDefName(constant.m_name))
            {
                const auto samplerPos = constant.m_name.rfind(SAMPLER_STR);
                if (samplerPos != std::string::npos)
                {
                    auto nameWithoutSamplerStr = constant.m_name;
                    nameWithoutSamplerStr.erase(samplerPos, std::char_traits<char>::length(SAMPLER_STR));
                    AddTextureDefName(nameWithoutSamplerStr);
                }
            }
        }
        else
            AddConstantName(constant.m_name);
    }
}

void AbstractMaterialConstantZoneStateDx11::ExtractNamesFromShader(const void* shader, const size_t shaderSize)
{
    const auto shaderInfo = d3d11::ShaderAnalyser::GetShaderInfo(static_cast<const uint8_t*>(shader), shaderSize);
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
                    AddTextureDefName(nameWithoutSamplerStr);
                }
            }
        }
    }
}
