#include "LoaderVertexDeclIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/TechsetConstantsIW4.h"

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
                    std::cerr << std::format("Failed to add vertex decl stream. Too many abbreviations: {}\n", assetName);
                    return AssetCreationResult::Failure();
                }

                std::string destinationAbbreviation;
                if (!NextAbbreviation(assetName, destinationAbbreviation, currentOffset))
                {
                    std::cerr << std::format("Failed to detect vertex decl destination abbreviation: {}\n", assetName);
                    return AssetCreationResult::Failure();
                }

                const auto foundSourceAbbreviation = std::ranges::find(materialStreamSourceAbbreviation, sourceAbbreviation);
                if (foundSourceAbbreviation == std::end(materialStreamSourceAbbreviation))
                {
                    std::cerr << std::format("Unknown vertex decl source abbreviation: {}\n", sourceAbbreviation);
                    return AssetCreationResult::Failure();
                }

                const auto foundDestinationAbbreviation = std::ranges::find(materialStreamDestinationAbbreviation, destinationAbbreviation);
                if (foundDestinationAbbreviation == std::end(materialStreamDestinationAbbreviation))
                {
                    std::cerr << std::format("Unknown vertex decl destination abbreviation: {}\n", destinationAbbreviation);
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

namespace IW4
{
    std::unique_ptr<AssetCreator<AssetVertexDecl>> CreateVertexDeclLoader(MemoryManager& memory)
    {
        return std::make_unique<LoaderVertexDecl>(memory);
    }
} // namespace IW4
