#include "PixelShaderLoaderT6.h"

#include "Game/T6/T6.h"
#include "Shader/ShaderCommon.h"

#include <format>
#include <iostream>

using namespace T6;

namespace
{
    class PixelShaderLoader final : public SubAssetCreator<SubAssetPixelShader>
    {
    public:
        PixelShaderLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateSubAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = shader::GetFileNameForPixelShaderAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* pixelShader = m_memory.Alloc<MaterialPixelShader>();
            pixelShader->name = m_memory.Dup(assetName.c_str());
            pixelShader->prog.loadDef.programSize = static_cast<decltype(GfxPixelShaderLoadDef::programSize)>(file.m_length);
            pixelShader->prog.ps = nullptr;

            auto* fileBuffer = m_memory.Alloc<char>(pixelShader->prog.loadDef.programSize);
            file.m_stream->read(fileBuffer, pixelShader->prog.loadDef.programSize);
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();

            pixelShader->prog.loadDef.program = fileBuffer;
            return AssetCreationResult::Success(context.AddSubAsset<SubAssetPixelShader>(assetName, pixelShader));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace techset
{
    std::unique_ptr<SubAssetCreator<SubAssetPixelShader>> CreatePixelShaderLoaderT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<PixelShaderLoader>(memory, searchPath);
    }
} // namespace techset
