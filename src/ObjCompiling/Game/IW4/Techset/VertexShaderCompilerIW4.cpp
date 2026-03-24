#include "VertexShaderCompilerIW4.h"

#include "Game/IW4/IW4.h"
#include "Shader/ShaderCommon.h"
#include "Techset/ShaderIncludeHandler.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <iostream>
#include <utility>

#ifdef _WIN32
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")
#endif

using namespace IW4;

namespace
{
    class VertexShaderCompilerIW4 final : public AssetCreator<AssetVertexShader>
    {
    public:
        VertexShaderCompilerIW4(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
#ifdef _WIN32
            const auto fileName = shader::GetSourceFileNameForShaderAssetName(assetName);
            auto file = m_search_path.Open(fileName);
            if (!file.IsOpen() || file.m_length <= 0)
                return AssetCreationResult::NoAction();

            if (std::cmp_greater(file.m_length, techset::ShaderIncludeHandler::MAX_SHADER_SIZE))
            {
                con::error("Invalid shader source \"{}\": File too big: {}", assetName, file.m_length);
                return AssetCreationResult::Failure();
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

            techset::ShaderIncludeHandler shaderIncluder(m_search_path);

            ID3DBlob* shaderBlob = nullptr;
            ID3DBlob* errorBlob = nullptr;
            const auto errorCode = D3DCompile(
                shaderData.get(), shaderSize, assetName.c_str(), nullptr, &shaderIncluder, "VSMain", "vs_3_0", shaderFlags, 0u, &shaderBlob, &errorBlob);

            if (FAILED(errorCode))
            {
                con::error("Invalid vertex shader \"{}\": Compilation error", assetName);

                if (errorBlob)
                {
                    std::cerr << "  " << static_cast<char*>(errorBlob->GetBufferPointer()) << "\n";
                    errorBlob->Release();
                }

                if (shaderBlob)
                    shaderBlob->Release();

                return AssetCreationResult::Failure();
            }

            con::info("Compiled vertex shader \"{}\"", assetName);

            const auto shaderBlobSize = static_cast<size_t>(shaderBlob->GetBufferSize());
            assert(shaderBlobSize % sizeof(uint32_t) == 0);

            auto* vertexShader = m_memory.Alloc<MaterialVertexShader>();
            vertexShader->name = m_memory.Dup(assetName.c_str());
            vertexShader->prog.loadDef.programSize = static_cast<uint16_t>(shaderBlobSize / sizeof(uint32_t));
            vertexShader->prog.loadDef.loadForRenderer = 0;
            vertexShader->prog.vs = nullptr;

            auto* assetShaderBuffer = m_memory.Alloc<char>(shaderBlobSize);
            memcpy(assetShaderBuffer, shaderBlob->GetBufferPointer(), shaderBlobSize);
            vertexShader->prog.loadDef.program = reinterpret_cast<uint32_t*>(assetShaderBuffer);

            shaderBlob->Release();

            return AssetCreationResult::Success(context.AddAsset(AssetRegistration<AssetVertexShader>(assetName, vertexShader)));
#else
            // Shader compilation is only support with Windows
            return AssetCreationResult::NoAction();
#endif
        }

        void FinalizeZone(AssetCreationContext& context) override {}

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace techset
{
    std::unique_ptr<IAssetCreator> CreateVertexShaderCompilerIW4(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<VertexShaderCompilerIW4>(memory, searchPath);
    }
} // namespace techset
