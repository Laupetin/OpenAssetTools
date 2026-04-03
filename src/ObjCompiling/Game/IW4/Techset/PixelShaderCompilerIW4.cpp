#include "PixelShaderCompilerIW4.h"

#include "Game/IW4/IW4.h"
#include "Shader/D3DShaderCompiler.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <iostream>
#include <utility>

using namespace IW4;

namespace
{
    class PixelShaderCompilerIW4 final : public AssetCreator<AssetPixelShader>
    {
    public:
        PixelShaderCompilerIW4(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            auto result = shader::CompileShader(assetName, "PSMain", "ps_3_0", false, m_search_path, m_memory);

            if (result.has_value())
            {
                const std::optional maybeShader(*std::move(result));
                if (!maybeShader.has_value())
                    return AssetCreationResult::NoAction();

                con::info("Compiled pixel shader \"{}\"", assetName);

                assert(maybeShader->m_shader_size % sizeof(uint32_t) == 0);

                auto* pixelShader = m_memory.Alloc<MaterialPixelShader>();
                pixelShader->name = m_memory.Dup(assetName.c_str());
                pixelShader->prog.loadDef.program = static_cast<unsigned int*>(maybeShader->m_shader_bin);
                pixelShader->prog.loadDef.programSize = static_cast<uint16_t>(maybeShader->m_shader_size / sizeof(uint32_t));

                return AssetCreationResult::Success(context.AddAsset(AssetRegistration<AssetPixelShader>(assetName, pixelShader)));
            }

            con::error("Failed to compile pixel shader \"{}\": {}", assetName, result.error());
            return AssetCreationResult::Failure();
        }

        void FinalizeZone(AssetCreationContext& context) override {}

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace techset
{
    std::unique_ptr<IAssetCreator> CreatePixelShaderCompilerIW4(MemoryManager& memory, ISearchPath& searchPath)
    {
        if (!shader::ShaderCompilationAvailable())
            return nullptr;

        return std::make_unique<PixelShaderCompilerIW4>(memory, searchPath);
    }
} // namespace techset
