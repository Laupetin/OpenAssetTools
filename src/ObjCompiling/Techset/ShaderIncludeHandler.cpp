#include "ShaderIncludeHandler.h"

#include "Shader/ShaderCommon.h"
#include "Utils/Logging/Log.h"

#include <iostream>
#include <utility>

#ifdef _WIN32

namespace techset
{
    ShaderIncludeHandler::ShaderIncludeHandler(ISearchPath& searchPath)
        : m_search_path(searchPath)
    {
    }

    HRESULT ShaderIncludeHandler::Open(D3D_INCLUDE_TYPE includeType, const LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
    {
        const auto fileName = shader::GetSourceFileNameForShaderAssetName(pFileName);
        auto file = m_search_path.Open(fileName);
        if (!file.IsOpen() || file.m_length <= 0)
            return E_FAIL;

        if (std::cmp_greater(file.m_length, MAX_SHADER_SIZE))
        {
            con::error("Invalid shader source \"{}\": File too big: {}\n", pFileName, file.m_length);
            return E_FAIL;
        }

        const auto shaderSize = static_cast<size_t>(file.m_length);
        auto shaderData = std::make_unique<char[]>(shaderSize);
        file.m_stream->read(shaderData.get(), static_cast<std::streamsize>(shaderSize));
        file.m_stream.reset();

        *ppData = shaderData.get();
        *pBytes = static_cast<UINT>(shaderSize);

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
} // namespace techset

#endif
