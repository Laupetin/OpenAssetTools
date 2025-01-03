#include "LoaderVertexShaderIW4.h"

#include "Game/IW4/IW4.h"

#include <cstdint>
#include <format>
#include <iostream>

using namespace IW4;

namespace
{
    class VertexShaderLoader final : public AssetCreator<AssetVertexShader>
    {
    public:
        VertexShaderLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = GetVertexShaderFileName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            if (file.m_length % sizeof(uint32_t) != 0)
            {
                std::cerr << std::format("Invalid vertex shader \"{}\": Size must be dividable by {}\n", assetName, sizeof(uint32_t));
                return AssetCreationResult::Failure();
            }

            auto* vertexShader = m_memory.Alloc<MaterialVertexShader>();
            vertexShader->name = m_memory.Dup(assetName.c_str());
            vertexShader->prog.loadDef.programSize = static_cast<uint16_t>(static_cast<size_t>(file.m_length) / sizeof(uint32_t));
            vertexShader->prog.loadDef.loadForRenderer = 0;
            vertexShader->prog.vs = nullptr;

            auto* fileBuffer = m_memory.Alloc<uint32_t>(vertexShader->prog.loadDef.programSize);
            file.m_stream->read(reinterpret_cast<char*>(fileBuffer), static_cast<std::streamsize>(vertexShader->prog.loadDef.programSize) * sizeof(uint32_t));
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();

            vertexShader->prog.loadDef.program = fileBuffer;
            return AssetCreationResult::Success(context.AddAsset<AssetVertexShader>(assetName, vertexShader));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW4
{
    std::string GetVertexShaderFileName(const std::string& vertexShaderAssetName)
    {
        return std::format("shader_bin/vs_{}.cso", vertexShaderAssetName);
    }

    std::unique_ptr<AssetCreator<AssetVertexShader>> CreateVertexShaderLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<VertexShaderLoader>(memory, searchPath);
    }
} // namespace IW4
