#include "CompilerVertexDeclIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/Techset/TechsetConstantsIW4.h"
#include "Techset/CommonVertexDeclCreator.h"
#include "Utils/Logging/Log.h"

using namespace IW4;

namespace
{
    class LoaderVertexDecl final : public AssetCreator<AssetVertexDecl>
    {
    public:
        explicit LoaderVertexDecl(MemoryManager& memory)
            : m_memory(memory)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto commonVertexDecl = techset::CreateVertexDeclFromName(assetName, commonRoutingInfos);
            if (!commonVertexDecl)
                return AssetCreationResult::Failure();

            if (commonVertexDecl->m_routing.size() > std::extent_v<decltype(MaterialVertexStreamRouting::data)>)
            {
                con::error("Vertex declaration can only have up to {} routing entries", std::extent_v<decltype(MaterialVertexStreamRouting::data)>);
                return AssetCreationResult::Failure();
            }

            auto* vertexDecl = m_memory.Alloc<MaterialVertexDeclaration>();

            vertexDecl->name = m_memory.Dup(assetName.c_str());

            for (const auto& commonRoutingEntry : commonVertexDecl->m_routing)
            {
                vertexDecl->routing.data[vertexDecl->streamCount].source = commonRoutingEntry.m_source;
                vertexDecl->routing.data[vertexDecl->streamCount].dest = commonRoutingEntry.m_destination;
                vertexDecl->hasOptionalSource = vertexDecl->hasOptionalSource || commonRoutingEntry.m_source >= STREAM_SRC_OPTIONAL_BEGIN;

                vertexDecl->streamCount++;
            }

            return AssetCreationResult::Success(context.AddAsset(AssetRegistration<AssetVertexDecl>(assetName, vertexDecl)));
        }

        MemoryManager& m_memory;
    };
} // namespace

namespace vertex_decl
{
    std::unique_ptr<AssetCreator<AssetVertexDecl>> CreateLoaderIW4(MemoryManager& memory)
    {
        return std::make_unique<LoaderVertexDecl>(memory);
    }
} // namespace vertex_decl
