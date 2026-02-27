#include "VertexShaderLoaderT6.h"

#include "Game/T6/T6.h"
#include "Shader/ShaderCommon.h"

#include <format>
#include <iostream>

using namespace T6;

namespace
{
    class VertexShaderLoader final : public SubAssetCreator<SubAssetVertexShader>
    {
    public:
        VertexShaderLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateSubAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = shader::GetFileNameForVertexShaderAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* vertexShader = m_memory.Alloc<MaterialVertexShader>();
            vertexShader->name = m_memory.Dup(assetName.c_str());
            vertexShader->prog.loadDef.programSize = static_cast<decltype(GfxVertexShaderLoadDef::programSize)>(file.m_length);
            vertexShader->prog.vs = nullptr;

            auto* fileBuffer = m_memory.Alloc<char>(vertexShader->prog.loadDef.programSize);
            file.m_stream->read(fileBuffer, vertexShader->prog.loadDef.programSize);
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();

            vertexShader->prog.loadDef.program = fileBuffer;
            return AssetCreationResult::Success(context.AddSubAsset<SubAssetVertexShader>(assetName, vertexShader));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace techset
{
    std::unique_ptr<SubAssetCreator<SubAssetVertexShader>> CreateVertexShaderLoaderT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<VertexShaderLoader>(memory, searchPath);
    }
} // namespace techset
