#include "VertexDeclCompilerT6.h"

#include "Game/T6/T6.h"
#include "Game/T6/Techset/TechsetConstantsT6.h"
#include "Techset/CommonVertexDeclCreator.h"
#include "Utils/Logging/Log.h"

using namespace T6;

namespace
{
    class VertexDeclCompilerT6 final : public SubAssetCreator<SubAssetVertexDecl>
    {
    public:
        explicit VertexDeclCompilerT6(MemoryManager& memory)
            : m_memory(memory)
        {
        }

        AssetCreationResult CreateSubAsset(const std::string& subAssetName, AssetCreationContext& context) override
        {
            const auto commonVertexDecl = techset::CreateVertexDeclFromName(subAssetName, commonRoutingInfos);
            if (!commonVertexDecl)
                return AssetCreationResult::Failure();

            if (commonVertexDecl->m_routing.size() > std::extent_v<decltype(MaterialVertexStreamRouting::data)>)
            {
                con::error("Vertex declaration can only have up to {} routing entries", std::extent_v<decltype(MaterialVertexStreamRouting::data)>);
                return AssetCreationResult::Failure();
            }

            auto* vertexDecl = m_memory.Alloc<MaterialVertexDeclaration>();

            for (const auto& commonRoutingEntry : commonVertexDecl->m_routing)
            {
                vertexDecl->routing.data[vertexDecl->streamCount].source = commonRoutingEntry.m_source;
                vertexDecl->routing.data[vertexDecl->streamCount].dest = commonRoutingEntry.m_destination;
                vertexDecl->hasOptionalSource = vertexDecl->hasOptionalSource || commonRoutingEntry.m_source >= STREAM_SRC_OPTIONAL_BEGIN;

                vertexDecl->streamCount++;
            }

            return AssetCreationResult::Success(context.AddSubAsset(AssetRegistration<SubAssetVertexDecl>(subAssetName, vertexDecl)));
        }

    private:
        MemoryManager& m_memory;
    };
} // namespace

namespace techset
{
    std::unique_ptr<ISubAssetCreator> CreateVertexDeclCompilerT6(MemoryManager& memory)
    {
        return std::make_unique<VertexDeclCompilerT6>(memory);
    }
} // namespace techset
