#include "AssetLoaderPixelShader.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"
#include "Shader/ShaderIncludeHandler.h"

#ifdef OS_TARGET_WINDOWS
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")
#endif

using namespace IW4;

void* AssetLoaderPixelShader::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* pixelShader = memory->Create<MaterialPixelShader>();
    memset(pixelShader, 0, sizeof(MaterialPixelShader));
    pixelShader->name = memory->Dup(assetName.c_str());
    return pixelShader;
}

bool AssetLoaderPixelShader::CanLoadFromRaw() const
{
    return true;
}

std::string AssetLoaderPixelShader::GetFileNameForSourceShader(const std::string& assetName)
{
    std::ostringstream ss;
    ss << "shader/" << assetName;
    return ss.str();
}

bool AssetLoaderPixelShader::LoadFromSource(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
{
#ifdef OS_TARGET_WINDOWS
    const auto fileName = GetFileNameForSourceShader(assetName);
    auto file = searchPath->Open(fileName);
    if (!file.IsOpen() || file.m_length <= 0)
        return false;

    if (file.m_length > ShaderIncludeHandler::MAX_SHADER_SIZE)
    {
        std::cerr << "Invalid pixel shader source \"" << assetName << "\": File too big: " << file.m_length << "\n";
        return false;
    }

    const auto shaderSize = static_cast<size_t>(file.m_length);
    const auto shaderData = std::make_unique<char[]>(shaderSize);
    file.m_stream->read(shaderData.get(), shaderSize);
    file.m_stream.reset();

    constexpr unsigned shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1
#ifdef _DEBUG
        | D3DCOMPILE_DEBUG
#endif
        ;

    ShaderIncludeHandler shaderIncluder(searchPath);

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    const auto errorCode = D3DCompile(shaderData.get(), shaderSize, assetName.c_str(), nullptr, &shaderIncluder, "PSMain", "ps_3_0", shaderFlags, 0u, &shaderBlob, &errorBlob);

    if (FAILED(errorCode))
    {
        std::cerr << "Invalid pixel shader \"" << assetName << "\": Compilation error\n";

        if (errorBlob)
        {
            std::cerr << "  " << static_cast<char*>(errorBlob->GetBufferPointer()) << "\n";
            errorBlob->Release();
        }

        if (shaderBlob)
            shaderBlob->Release();

        return false;
    }

    std::cout << "Compiled pixel shader \"" << assetName << "\"\n";

    const auto shaderBlobSize = static_cast<size_t>(shaderBlob->GetBufferSize());
    assert(shaderBlobSize % sizeof(uint32_t) == 0);

    auto* pixelShader = memory->Create<MaterialPixelShader>();
    pixelShader->name = memory->Dup(assetName.c_str());
    pixelShader->prog.loadDef.programSize = static_cast<uint16_t>(shaderBlobSize / sizeof(uint32_t));
    pixelShader->prog.loadDef.loadForRenderer = 0;
    pixelShader->prog.ps = nullptr;

    auto* assetShaderBuffer = static_cast<char*>(memory->Alloc(shaderBlobSize));
    memcpy(assetShaderBuffer, shaderBlob->GetBufferPointer(), shaderBlobSize);
    pixelShader->prog.loadDef.program = reinterpret_cast<uint32_t*>(assetShaderBuffer);
    manager->AddAsset(ASSET_TYPE_PIXELSHADER, assetName, pixelShader);

    shaderBlob->Release();

    return true;
#else
    // Shader compilation is only support with Windows
    return false;
#endif
}

std::string AssetLoaderPixelShader::GetFileNameForCompiledShader(const std::string& assetName)
{
    std::ostringstream ss;
    ss << "shader_bin/ps_" << assetName << ".cso";
    return ss.str();
}

bool AssetLoaderPixelShader::LoadCompiled(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
{
    const auto fileName = GetFileNameForCompiledShader(assetName);
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    if(file.m_length % sizeof(uint32_t) != 0)
    {
        std::cerr << "Invalid pixel shader \"" << assetName << "\": Size must be dividable by " << sizeof(uint32_t) << "\n";
        return false;
    }

    auto* pixelShader = memory->Create<MaterialPixelShader>();
    pixelShader->name = memory->Dup(assetName.c_str());
    pixelShader->prog.loadDef.programSize = static_cast<uint16_t>(static_cast<size_t>(file.m_length) / sizeof(uint32_t));
    pixelShader->prog.loadDef.loadForRenderer = 0;
    pixelShader->prog.ps = nullptr;

    auto* fileBuffer = static_cast<char*>(memory->Alloc(pixelShader->prog.loadDef.programSize * sizeof(uint32_t)));
    file.m_stream->read(fileBuffer, static_cast<std::streamsize>(pixelShader->prog.loadDef.programSize) * sizeof(uint32_t));
    if (file.m_stream->gcount() != file.m_length)
        return false;

    pixelShader->prog.loadDef.program = reinterpret_cast<uint32_t*>(fileBuffer);
    manager->AddAsset(ASSET_TYPE_PIXELSHADER, assetName, pixelShader);

    return true;
}

bool AssetLoaderPixelShader::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    return LoadFromSource(assetName, searchPath, memory, manager) || LoadCompiled(assetName, searchPath, memory, manager);
}
