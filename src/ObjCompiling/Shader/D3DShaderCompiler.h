#pragma once

#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <expected>
#include <optional>
#include <string>

namespace shader
{
    class CompiledShader
    {
    public:
        void* m_shader_bin;
        size_t m_shader_size;
    };

    bool ShaderCompilationAvailable();

    std::expected<std::optional<CompiledShader>, std::string> CompileShader(
        const std::string& shaderFile, const std::string& entryPoint, const std::string& target, bool debug, ISearchPath& searchPath, MemoryManager& memory);
} // namespace shader
