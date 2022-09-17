#include "AssetLoaderVertexShader.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

#ifdef OS_TARGET_WINDOWS
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")
#endif

using namespace IW4;

class ShaderIncluder : public ID3DInclude
{
    static constexpr auto MAX_SHADER_SIZE = 0x1900000u;

public:
    ShaderIncluder(ISearchPath* searchPath)
        : m_search_path(searchPath)
    {
    }

    virtual ~ShaderIncluder() = default;

    HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override
    {
        std::ostringstream ss;
        ss << "shaders/" << pFileName;

        auto file = m_search_path->Open(ss.str());
        if (!file.IsOpen() || file.m_length <= 0)
            return E_FAIL;

        if (file.m_length > MAX_SHADER_SIZE)
        {
            std::cerr << "Invalid shader source \"" << pFileName << "\": File too big: " << file.m_length << "\n";
            return false;
        }

        const auto shaderSize = static_cast<size_t>(file.m_length);
        auto shaderData = std::make_unique<char[]>(shaderSize);
        file.m_stream->read(shaderData.get(), shaderSize);
        file.m_stream.reset();

        *ppData = shaderData.get();
        *pBytes = shaderSize;

        m_file_buffers_in_use.push_back(std::move(shaderData));

        return S_OK;
    }

    HRESULT __stdcall Close(LPCVOID pData) override
    {
        for (auto i = m_file_buffers_in_use.begin(); i != m_file_buffers_in_use.end(); ++i)
        {
            if (i->get() == pData)
            {
                m_file_buffers_in_use.erase(i);
                return S_OK;
            }
        }

        return E_FAIL;
    }

private:
    ISearchPath* m_search_path;
    std::vector<std::unique_ptr<char[]>> m_file_buffers_in_use;
};

void* AssetLoaderVertexShader::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* vertexShader = memory->Create<MaterialVertexShader>();
    memset(vertexShader, 0, sizeof(MaterialVertexShader));
    vertexShader->name = memory->Dup(assetName.c_str());
    return vertexShader;
}

bool AssetLoaderVertexShader::CanLoadFromRaw() const
{
    return true;
}

std::string AssetLoaderVertexShader::GetFileNameForSourceShader(const std::string& assetName)
{
    std::ostringstream ss;
    ss << "shader/" << assetName;
    return ss.str();
}

bool AssetLoaderVertexShader::LoadFromSource(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone)
{
#ifdef OS_TARGET_WINDOWS
    const auto fileName = GetFileNameForSourceShader(assetName);
    auto file = searchPath->Open(fileName);
    if (!file.IsOpen() || file.m_length <= 0)
        return false;

    if (file.m_length > MAX_SHADER_SIZE)
    {
        std::cerr << "Invalid vertex shader source \"" << assetName << "\": File too big: " << file.m_length << "\n";
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

    ShaderIncluder shaderIncluder(searchPath);

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    const auto errorCode = D3DCompile(shaderData.get(), shaderSize, assetName.c_str(), nullptr, &shaderIncluder, "VSMain", "vs_3_0", shaderFlags, 0u, &shaderBlob, &errorBlob);

    if (FAILED(errorCode))
    {
        std::cerr << "Invalid vertex shader \"" << assetName << "\": Compilation error\n";

        if (errorBlob)
        {
            std::cerr << "  " << static_cast<char*>(errorBlob->GetBufferPointer()) << "\n";
            errorBlob->Release();
        }

        if (shaderBlob)
            shaderBlob->Release();

        return false;
    }

    std::cout << "Compiled vertex shader \"" << assetName << "\"\n";

    const auto shaderBlobSize = static_cast<size_t>(shaderBlob->GetBufferSize());
    assert(shaderBlobSize % sizeof(uint32_t) == 0);

    auto* vertexShader = memory->Create<MaterialVertexShader>();
    vertexShader->name = memory->Dup(assetName.c_str());
    vertexShader->prog.loadDef.programSize = static_cast<uint16_t>(shaderBlobSize / sizeof(uint32_t));
    vertexShader->prog.loadDef.loadForRenderer = 0;
    vertexShader->prog.vs = nullptr;

    auto* assetShaderBuffer = static_cast<char*>(memory->Alloc(shaderBlobSize));
    memcpy(assetShaderBuffer, shaderBlob->GetBufferPointer(), shaderBlobSize);
    vertexShader->prog.loadDef.program = reinterpret_cast<uint32_t*>(assetShaderBuffer);
    manager->AddAsset(ASSET_TYPE_VERTEXSHADER, assetName, vertexShader);

    shaderBlob->Release();

    return true;
#else
    // Shader compilation is only support with Windows
    return false;
#endif
}

std::string AssetLoaderVertexShader::GetFileNameForCompiledShader(const std::string& assetName)
{
    std::ostringstream ss;
    ss << "shader_bin/vs_" << assetName << ".cso";
    return ss.str();
}

bool AssetLoaderVertexShader::LoadCompiled(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone)
{
    const auto fileName = GetFileNameForCompiledShader(assetName);
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    if (file.m_length % sizeof(uint32_t) != 0)
    {
        std::cerr << "Invalid compiled vertex shader \"" << assetName << "\": Size must be dividable by " << sizeof(uint32_t) << "\n";
        return false;
    }

    auto* vertexShader = memory->Create<MaterialVertexShader>();
    vertexShader->name = memory->Dup(assetName.c_str());
    vertexShader->prog.loadDef.programSize = static_cast<uint16_t>(static_cast<size_t>(file.m_length) / sizeof(uint32_t));
    vertexShader->prog.loadDef.loadForRenderer = 0;
    vertexShader->prog.vs = nullptr;

    auto* fileBuffer = static_cast<char*>(memory->Alloc(vertexShader->prog.loadDef.programSize * sizeof(uint32_t)));
    file.m_stream->read(fileBuffer, static_cast<std::streamsize>(vertexShader->prog.loadDef.programSize) * sizeof(uint32_t));
    if (file.m_stream->gcount() != file.m_length)
        return false;

    vertexShader->prog.loadDef.program = reinterpret_cast<uint32_t*>(fileBuffer);
    manager->AddAsset(ASSET_TYPE_VERTEXSHADER, assetName, vertexShader);

    return true;
}

bool AssetLoaderVertexShader::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    return LoadFromSource(assetName, searchPath, memory, manager, zone) || LoadCompiled(assetName, searchPath, memory, manager, zone);
}
