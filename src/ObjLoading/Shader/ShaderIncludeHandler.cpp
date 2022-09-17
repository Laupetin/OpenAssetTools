#include "ShaderIncludeHandler.h"

#include <iostream>
#include <sstream>

#ifdef OS_TARGET_WINDOWS

ShaderIncludeHandler::ShaderIncludeHandler(ISearchPath* searchPath)
    : m_search_path(searchPath)
{
}

ShaderIncludeHandler::~ShaderIncludeHandler() = default;

HRESULT ShaderIncludeHandler::Open(D3D_INCLUDE_TYPE includeType, const LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
    std::ostringstream ss;
    ss << "shaders/" << pFileName;

    auto file = m_search_path->Open(ss.str());
    if (!file.IsOpen() || file.m_length <= 0)
        return E_FAIL;

    if (file.m_length > MAX_SHADER_SIZE)
    {
        std::cerr << "Invalid shader source \"" << pFileName << "\": File too big: " << file.m_length << "\n";
        return E_FAIL;
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

HRESULT ShaderIncludeHandler::Close(const LPCVOID pData)
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

#endif
