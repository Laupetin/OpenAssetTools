#pragma once

#include "SearchPath/ISearchPath.h"

// Shader compilation only available on Windows
#ifdef _WIN32
#include <d3dcompiler.h>

namespace techset
{
    class ShaderIncludeHandler : public ID3DInclude
    {
    public:
        static constexpr size_t MAX_SHADER_SIZE = 0x1900000u;

        explicit ShaderIncludeHandler(ISearchPath& searchPath);
        virtual ~ShaderIncludeHandler() = default;

        HRESULT __declspec(nothrow) __stdcall Open(D3D_INCLUDE_TYPE includeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override;
        HRESULT __declspec(nothrow) __stdcall Close(LPCVOID pData) override;

    private:
        ISearchPath& m_search_path;
        std::vector<std::unique_ptr<char[]>> m_file_buffers_in_use;
    };
} // namespace techset
#endif
