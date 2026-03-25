#include "D3DShaderCompiler.h"

#include "Shader/ShaderCommon.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <utility>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{
    // The types based on vkd3d which is licensed under the
    // GNU Lesser General Public License as published by the Free Software Foundation
    // All types are prefixed with OAT_ to not cause conflicts as the original definition may be included
    // depending on the platform.
    // Types were duplicated to be available on any platform and to not require the corresponding library
    // to be installed

    typedef int OAT_HRESULT;
#define OAT_SUCCEEDED(hr) ((OAT_HRESULT)(hr) >= 0)
#define OAT_FAILED(hr) ((OAT_HRESULT)(hr) < 0)

    typedef unsigned int OAT_ULONG;
    typedef unsigned long OAT_ULONG_PTR;
    typedef OAT_ULONG_PTR OAT_SIZE_T;

#define OAT_HRESULT_TYPEDEF(x) ((OAT_HRESULT)x)

#define OAT_S_OK OAT_HRESULT_TYPEDEF(0)
#define OAT_S_FALSE OAT_HRESULT_TYPEDEF(1)

#define OAT_E_FAIL OAT_HRESULT_TYPEDEF(0x80004005)

#define OAT_D3DCOMPILE_DEBUG 0x00000001
#define OAT_D3DCOMPILE_SKIP_VALIDATION 0x00000002
#define OAT_D3DCOMPILE_SKIP_OPTIMIZATION 0x00000004
#define OAT_D3DCOMPILE_PACK_MATRIX_ROW_MAJOR 0x00000008
#define OAT_D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR 0x00000010
#define OAT_D3DCOMPILE_PARTIAL_PRECISION 0x00000020
#define OAT_D3DCOMPILE_FORCE_VS_SOFTWARE_NO_OPT 0x00000040
#define OAT_D3DCOMPILE_FORCE_PS_SOFTWARE_NO_OPT 0x00000080
#define OAT_D3DCOMPILE_NO_PRESHADER 0x00000100
#define OAT_D3DCOMPILE_AVOID_FLOW_CONTROL 0x00000200
#define OAT_D3DCOMPILE_PREFER_FLOW_CONTROL 0x00000400
#define OAT_D3DCOMPILE_ENABLE_STRICTNESS 0x00000800
#define OAT_D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY 0x00001000
#define OAT_D3DCOMPILE_IEEE_STRICTNESS 0x00002000
#define OAT_D3DCOMPILE_OPTIMIZATION_LEVEL0 0x00004000
#define OAT_D3DCOMPILE_OPTIMIZATION_LEVEL1 0x00000000
#define OAT_D3DCOMPILE_OPTIMIZATION_LEVEL2 0x0000c000
#define OAT_D3DCOMPILE_OPTIMIZATION_LEVEL3 0x00008000
#define OAT_D3DCOMPILE_RESERVED16 0x00010000
#define OAT_D3DCOMPILE_RESERVED17 0x00020000
#define OAT_D3DCOMPILE_WARNINGS_ARE_ERRORS 0x00040000
#define OAT_D3DCOMPILE_RESOURCES_MAY_ALIAS 0x00080000
#define OAT_D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES 0x00100000
#define OAT_D3DCOMPILE_ALL_RESOURCES_BOUND 0x00200000
#define OAT_D3DCOMPILE_DEBUG_NAME_FOR_SOURCE 0x00400000
#define OAT_D3DCOMPILE_DEBUG_NAME_FOR_BINARY 0x00800000

    enum OAT_D3D_INCLUDE_TYPE : uint32_t
    {
        OAT_D3D_INCLUDE_LOCAL = 0,
        OAT_D3D_INCLUDE_SYSTEM = 1
    };

    struct OAT_ID3DInclude
    {
        virtual OAT_HRESULT __stdcall Open(
            OAT_D3D_INCLUDE_TYPE includeType, const char* fileName, const void* parentData, const void** data, unsigned int* size) = 0;
        virtual OAT_HRESULT __stdcall Close(const void* data) = 0;
    };

    struct OAT_GUID
    {
        unsigned int Data1;
        unsigned short Data2;
        unsigned short Data3;
        unsigned char Data4[8];
    };

    typedef OAT_GUID OAT_IID;

    struct OAT_IUnknown
    {
        virtual OAT_HRESULT __stdcall QueryInterface(const OAT_IID& riid, void** object) = 0;

        virtual OAT_ULONG __stdcall AddRef() = 0;

        virtual OAT_ULONG __stdcall Release() = 0;
    };

    struct OAT_ID3DBlob : OAT_IUnknown
    {
        virtual void* __stdcall GetBufferPointer() = 0;

        virtual OAT_SIZE_T __stdcall GetBufferSize() = 0;
    };

    struct OAT_D3D_SHADER_MACRO
    {
        const char* Name;
        const char* Definition;
    };

    typedef OAT_HRESULT(__stdcall* D3DCompile_t)(const void* data,
                                                 OAT_SIZE_T dataSize,
                                                 const char* filename,
                                                 const OAT_D3D_SHADER_MACRO* defines,
                                                 OAT_ID3DInclude* include,
                                                 const char* entrypoint,
                                                 const char* target,
                                                 unsigned int flags,
                                                 unsigned int effectFlags,
                                                 OAT_ID3DBlob** shader,
                                                 OAT_ID3DBlob** errorMessages);

    constexpr size_t MAX_SHADER_SIZE = 0x1900000u;

    class ShaderIncludeHandler : public OAT_ID3DInclude
    {
    public:
        explicit ShaderIncludeHandler(ISearchPath& searchPath)
            : m_search_path(searchPath)
        {
        }

        OAT_HRESULT
        __stdcall Open(OAT_D3D_INCLUDE_TYPE includeType, const char* fileName, const void* parentData, const void** data, unsigned int* size) override
        {
            const auto fullFileName = shader::GetSourceFileNameForShaderAssetName(fileName);
            auto file = m_search_path.Open(fullFileName);
            if (!file.IsOpen() || file.m_length <= 0)
                return OAT_E_FAIL;

            if (std::cmp_greater(file.m_length, MAX_SHADER_SIZE))
            {
                con::error("Invalid shader source \"{}\": File too big: {}\n", fileName, file.m_length);
                return OAT_E_FAIL;
            }

            const auto shaderSize = static_cast<size_t>(file.m_length);
            auto shaderData = std::make_unique<char[]>(shaderSize);
            file.m_stream->read(shaderData.get(), static_cast<std::streamsize>(shaderSize));
            file.m_stream.reset();

            *data = shaderData.get();
            *size = static_cast<unsigned int>(shaderSize);

            m_file_buffers_in_use.push_back(std::move(shaderData));

            return OAT_S_OK;
        }

        OAT_HRESULT __stdcall Close(const void* data) override
        {
            for (auto i = m_file_buffers_in_use.begin(); i != m_file_buffers_in_use.end(); ++i)
            {
                if (i->get() == data)
                {
                    m_file_buffers_in_use.erase(i);
                    return OAT_S_OK;
                }
            }

            return OAT_E_FAIL;
        }

    private:
        ISearchPath& m_search_path;
        std::vector<std::unique_ptr<char[]>> m_file_buffers_in_use;
    };

    bool initialized = false;
    bool compilationAvailable = false;
    D3DCompile_t d3dCompile = nullptr;

    void PrintInitializationFailedMessage()
    {
#ifdef _WIN32
        con::warn("Could not initialize shader compilation. Make sure DirectX is installed on your machine if you want to make use of it.");
#endif
    }

    void InitializeShaderCompilation()
    {
        initialized = true;

#ifdef _WIN32
        const auto d3dCompiler = LoadLibraryA("D3DCOMPILER_47.dll");
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

        d3dCompile = reinterpret_cast<D3DCompile_t>(d3dCompileAddress);
        compilationAvailable = true;
#endif
    }
} // namespace

namespace shader
{
    bool ShaderCompilationAvailable()
    {
        if (!initialized)
            InitializeShaderCompilation();

        return compilationAvailable;
    }

    result::Expected<std::optional<CompiledShader>, std::string> CompileShader(const std::string& shaderFile,
                                                                               const std::string& entryPoint,
                                                                               const std::string& target,
                                                                               const bool debug,
                                                                               ISearchPath& searchPath,
                                                                               MemoryManager& memory)
    {
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

        unsigned shaderFlags = OAT_D3DCOMPILE_OPTIMIZATION_LEVEL1;
        if (debug)
            shaderFlags |= OAT_D3DCOMPILE_DEBUG;

        ShaderIncludeHandler shaderIncluder(searchPath);

        OAT_ID3DBlob* shaderBlob = nullptr;
        OAT_ID3DBlob* errorBlob = nullptr;
        const auto errorCode = d3dCompile(shaderData.get(),
                                          static_cast<OAT_SIZE_T>(shaderSize),
                                          fileName.c_str(),
                                          nullptr,
                                          &shaderIncluder,
                                          entryPoint.c_str(),
                                          target.c_str(),
                                          shaderFlags,
                                          0u,
                                          &shaderBlob,
                                          &errorBlob);

        if (OAT_FAILED(errorCode))
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
    }
} // namespace shader
