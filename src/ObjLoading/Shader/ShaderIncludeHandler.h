#pragma once

#include "SearchPath/ISearchPath.h"

// Shader compilation only available on Windows
#ifdef OS_TARGET_WINDOWS
#include <d3dcompiler.h>

class ShaderIncludeHandler : public ID3DInclude
{
public:
    static constexpr auto MAX_SHADER_SIZE = 0x1900000u;

    ShaderIncludeHandler(ISearchPath* searchPath);

    virtual ~ShaderIncludeHandler();

    HRESULT __declspec(nothrow) __stdcall Open(D3D_INCLUDE_TYPE includeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override;
    HRESULT __declspec(nothrow) __stdcall Close(LPCVOID pData) override;

private:
    ISearchPath* m_search_path;
    std::vector<std::unique_ptr<char[]>> m_file_buffers_in_use;
};
#endif