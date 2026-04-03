#include "D3DShaderCompiler.h"

#include "Shader/ShaderCommon.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <utility>

#ifdef _WIN32
#include <d3dcompiler.h>
#endif

namespace
{
#ifdef _WIN32
    constexpr size_t MAX_SHADER_SIZE = 0x1900000u;

    class ShaderIncludeHandler : public ID3DInclude
    {
    public:
        explicit ShaderIncludeHandler(ISearchPath& searchPath)
            : m_search_path(searchPath)
        {
        }

        HRESULT __stdcall Open(D3D_INCLUDE_TYPE includeType, const char* fileName, const void* parentData, const void** data, unsigned int* size) override
        {
            const auto fullFileName = shader::GetSourceFileNameForShaderAssetName(fileName);
            auto file = m_search_path.Open(fullFileName);
            if (!file.IsOpen() || file.m_length <= 0)
                return E_FAIL;

            if (std::cmp_greater(file.m_length, MAX_SHADER_SIZE))
            {
                con::error("Invalid shader source \"{}\": File too big: {}\n", fileName, file.m_length);
                return E_FAIL;
            }

            const auto shaderSize = static_cast<size_t>(file.m_length);
            auto shaderData = std::make_unique<char[]>(shaderSize);
            file.m_stream->read(shaderData.get(), static_cast<std::streamsize>(shaderSize));
            file.m_stream.reset();

            *data = shaderData.get();
            *size = static_cast<unsigned int>(shaderSize);

            m_file_buffers_in_use.push_back(std::move(shaderData));

            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE Close(const void* data) override
        {
            for (auto i = m_file_buffers_in_use.begin(); i != m_file_buffers_in_use.end(); ++i)
            {
                if (i->get() == data)
                {
                    m_file_buffers_in_use.erase(i);
                    return S_OK;
                }
            }

            return E_FAIL;
        }

    private:
        ISearchPath& m_search_path;
        std::vector<std::unique_ptr<char[]>> m_file_buffers_in_use;
    };

    bool initialized = false;
    bool compilationAvailable = false;
    pD3DCompile d3dCompile = nullptr;

    void PrintInitializationFailedMessage()
    {
        con::warn("Could not initialize shader compilation. Make sure DirectX is installed on your machine if you want to make use of it.");
    }

#endif

    void InitializeShaderCompilation()
    {
#ifdef _WIN32
        initialized = true;

        const auto d3dCompiler = LoadLibraryA("d3dcompiler_47.dll");
        if (!d3dCompiler)
        {
            PrintInitializationFailedMessage();
            return;
        }

        const auto d3dCompileAddress = GetProcAddress(d3dCompiler, "D3DCompile");
        if (!d3dCompileAddress)
        {
            PrintInitializationFailedMessage();
            return;
        }

        d3dCompile = reinterpret_cast<pD3DCompile>(d3dCompileAddress);
        compilationAvailable = true;
#endif
    }
} // namespace

namespace shader
{
    bool ShaderCompilationAvailable()
    {
#ifdef _WIN32
        if (!initialized)
            InitializeShaderCompilation();

        return compilationAvailable;
#else
        return false;
#endif
    }

    result::Expected<std::optional<CompiledShader>, std::string> CompileShader(const std::string& shaderFile,
                                                                               const std::string& entryPoint,
                                                                               const std::string& target,
                                                                               const bool debug,
                                                                               ISearchPath& searchPath,
                                                                               MemoryManager& memory)
    {
#ifdef _WIN32
        if (!initialized)
            InitializeShaderCompilation();
        if (!compilationAvailable)
            return result::Unexpected<std::string>("Shader compilation unavailable");

        const auto fileName = GetSourceFileNameForShaderAssetName(shaderFile);
        auto file = searchPath.Open(fileName);
        if (!file.IsOpen() || file.m_length <= 0)
            return std::optional<CompiledShader>(std::nullopt);

        if (std::cmp_greater(file.m_length, MAX_SHADER_SIZE))
            return result::Unexpected(std::format("File too big: {}", file.m_length));

        const auto shaderSize = file.m_length;
        const auto shaderData = std::make_unique<char[]>(static_cast<size_t>(shaderSize));
        file.m_stream->read(shaderData.get(), static_cast<std::streamsize>(shaderSize));
        file.m_stream.reset();

        unsigned shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
        if (debug)
            shaderFlags |= D3DCOMPILE_DEBUG;

        ShaderIncludeHandler shaderIncluder(searchPath);

        ID3DBlob* shaderBlob = nullptr;
        ID3DBlob* errorBlob = nullptr;
        const auto errorCode = d3dCompile(shaderData.get(),
                                          static_cast<SIZE_T>(shaderSize),
                                          fileName.c_str(),
                                          nullptr,
                                          &shaderIncluder,
                                          entryPoint.c_str(),
                                          target.c_str(),
                                          shaderFlags,
                                          0u,
                                          &shaderBlob,
                                          &errorBlob);

        if (FAILED(errorCode))
        {
            std::string errorMessage;

            if (errorBlob)
            {
                errorMessage = std::format("Compilation error: {}", static_cast<const char*>(errorBlob->GetBufferPointer()));
                errorBlob->Release();
            }
            else
            {
                errorMessage = "Unknown compilation error";
            }

            if (shaderBlob)
                shaderBlob->Release();

            return result::Unexpected(std::move(errorMessage));
        }

        const auto shaderBlobSize = static_cast<size_t>(shaderBlob->GetBufferSize());
        auto* assetShaderBuffer = memory.Alloc<char>(shaderBlobSize);
        memcpy(assetShaderBuffer, shaderBlob->GetBufferPointer(), shaderBlobSize);

        shaderBlob->Release();

        return std::optional(CompiledShader{
            .m_shader_bin = assetShaderBuffer,
            .m_shader_size = shaderBlobSize,
        });
#else
        return result::Unexpected<std::string>("Shader compilation unavailable");
#endif
    }
} // namespace shader
