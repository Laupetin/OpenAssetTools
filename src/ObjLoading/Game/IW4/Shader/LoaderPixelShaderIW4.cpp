#include "LoaderPixelShaderIW4.h"

#include "Game/IW4/IW4.h"
#include "Shader/ShaderCommon.h"
#include "Utils/Logging/Log.h"

#include <cstdint>
#include <format>
#include <iostream>

using namespace IW4;

namespace
{
    class PixelShaderLoader final : public AssetCreator<AssetPixelShader>
    {
    public:
        PixelShaderLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = shader::GetFileNameForPixelShaderAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            if (file.m_length % sizeof(uint32_t) != 0)
            {
                con::error("Invalid pixel shader \"{}\": Size must be dividable by {}", assetName, sizeof(uint32_t));
                return AssetCreationResult::Failure();
            }

            auto* pixelShader = m_memory.Alloc<MaterialPixelShader>();
            pixelShader->name = m_memory.Dup(assetName.c_str());
            pixelShader->prog.loadDef.programSize = static_cast<uint16_t>(static_cast<size_t>(file.m_length) / sizeof(uint32_t));
            pixelShader->prog.loadDef.loadForRenderer = 0;
            pixelShader->prog.ps = nullptr;

            auto* fileBuffer = m_memory.Alloc<uint32_t>(pixelShader->prog.loadDef.programSize);
            file.m_stream->read(reinterpret_cast<char*>(fileBuffer), static_cast<std::streamsize>(pixelShader->prog.loadDef.programSize) * sizeof(uint32_t));
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();

            pixelShader->prog.loadDef.program = fileBuffer;
            return AssetCreationResult::Success(context.AddAsset<AssetPixelShader>(assetName, pixelShader));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace shader
{
    std::unique_ptr<AssetCreator<AssetPixelShader>> CreatePixelShaderLoaderIW4(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<PixelShaderLoader>(memory, searchPath);
    }
} // namespace shader
