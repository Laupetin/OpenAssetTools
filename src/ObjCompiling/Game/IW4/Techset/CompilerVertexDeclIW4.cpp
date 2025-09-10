#include "CompilerVertexDeclIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/TechsetConstantsIW4.h"
#include "Utils/Logging/Log.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace IW4;

namespace
{
    class LoaderVertexDecl final : public AssetCreator<AssetVertexDecl>
    {
    public:
        LoaderVertexDecl(MemoryManager& memory)
            : m_memory(memory)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            auto* decl = m_memory.Alloc<MaterialVertexDeclaration>();
            decl->name = m_memory.Dup(assetName.c_str());

            size_t currentOffset = 0u;

            std::string sourceAbbreviation;
            while (NextAbbreviation(assetName, sourceAbbreviation, currentOffset))
            {
                if (decl->streamCount >= std::extent_v<decltype(MaterialVertexStreamRouting::data)>)
                {
                    con::error("Failed to add vertex decl stream. Too many abbreviations: {}", assetName);
                    return AssetCreationResult::Failure();
                }

                std::string destinationAbbreviation;
                if (!NextAbbreviation(assetName, destinationAbbreviation, currentOffset))
                {
                    con::error("Failed to detect vertex decl destination abbreviation: {}", assetName);
                    return AssetCreationResult::Failure();
                }

                const auto foundSourceAbbreviation = std::ranges::find(materialStreamSourceAbbreviation, sourceAbbreviation);
                if (foundSourceAbbreviation == std::end(materialStreamSourceAbbreviation))
                {
                    con::error("Unknown vertex decl source abbreviation: {}", sourceAbbreviation);
                    return AssetCreationResult::Failure();
                }

                const auto foundDestinationAbbreviation = std::ranges::find(materialStreamDestinationAbbreviation, destinationAbbreviation);
                if (foundDestinationAbbreviation == std::end(materialStreamDestinationAbbreviation))
                {
                    con::error("Unknown vertex decl destination abbreviation: {}", destinationAbbreviation);
                    return AssetCreationResult::Failure();
                }

                const auto sourceIndex = static_cast<MaterialStreamStreamSource_e>(foundSourceAbbreviation - std::begin(materialStreamSourceAbbreviation));
                const auto destinationIndex =
                    static_cast<MaterialStreamDestination_e>(foundDestinationAbbreviation - std::begin(materialStreamDestinationAbbreviation));

                decl->routing.data[decl->streamCount].source = sourceIndex;
                decl->routing.data[decl->streamCount].dest = destinationIndex;
                decl->hasOptionalSource = decl->hasOptionalSource || sourceIndex >= STREAM_SRC_OPTIONAL_BEGIN;
                decl->streamCount++;
            }

            return AssetCreationResult::Success(context.AddAsset<AssetVertexDecl>(assetName, decl));
        }

        static bool NextAbbreviation(const std::string& assetName, std::string& abbreviation, size_t& offset)
        {
            if (offset >= assetName.size())
                return false;

            if (offset + 1 < assetName.size() && isdigit(assetName[offset + 1]))
            {
                abbreviation = std::string(assetName, offset, 2);
                offset += 2;
            }
            else
            {
                abbreviation = std::string(assetName, offset, 1);
                offset += 1;
            }

            return true;
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
