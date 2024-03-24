#include "AssetLoaderVertexDecl.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/TechsetConstantsIW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <iostream>

using namespace IW4;

void* AssetLoaderVertexDecl::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* vertexDecl = memory->Create<MaterialVertexDeclaration>();
    memset(vertexDecl, 0, sizeof(MaterialVertexDeclaration));
    vertexDecl->name = memory->Dup(assetName.c_str());
    return vertexDecl;
}

bool AssetLoaderVertexDecl::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderVertexDecl::NextAbbreviation(const std::string& assetName, std::string& abbreviation, size_t& offset)
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

bool AssetLoaderVertexDecl::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    MaterialVertexDeclaration decl{};

    size_t currentOffset = 0u;
    std::string sourceAbbreviation;
    while (NextAbbreviation(assetName, sourceAbbreviation, currentOffset))
    {
        if (decl.streamCount >= std::extent_v<decltype(MaterialVertexStreamRouting::data)>)
        {
            std::cout << "Failed to add vertex decl stream. Too many abbreviations: " << assetName << "\n";
            return false;
        }

        std::string destinationAbbreviation;
        if (!NextAbbreviation(assetName, destinationAbbreviation, currentOffset))
        {
            std::cout << "Failed to detect vertex decl destination abbreviation: " << assetName << "\n";
            return false;
        }

        const auto foundSourceAbbreviation = std::ranges::find(materialStreamSourceAbbreviation, sourceAbbreviation);
        if (foundSourceAbbreviation == std::end(materialStreamSourceAbbreviation))
        {
            std::cout << "Unknown vertex decl source abbreviation: " << sourceAbbreviation << "\n";
            return false;
        }

        const auto foundDestinationAbbreviation = std::ranges::find(materialStreamDestinationAbbreviation, destinationAbbreviation);
        if (foundDestinationAbbreviation == std::end(materialStreamDestinationAbbreviation))
        {
            std::cout << "Unknown vertex decl destination abbreviation: " << destinationAbbreviation << "\n";
            return false;
        }

        const auto sourceIndex = static_cast<MaterialStreamStreamSource_e>(foundSourceAbbreviation - std::begin(materialStreamSourceAbbreviation));
        const auto destinationIndex =
            static_cast<MaterialStreamDestination_e>(foundDestinationAbbreviation - std::begin(materialStreamDestinationAbbreviation));

        decl.routing.data[decl.streamCount].source = sourceIndex;
        decl.routing.data[decl.streamCount].dest = destinationIndex;
        decl.hasOptionalSource = decl.hasOptionalSource || sourceIndex >= STREAM_SRC_OPTIONAL_BEGIN;
        decl.streamCount++;
    }

    decl.name = memory->Dup(assetName.c_str());

    auto* allocatedDecl = memory->Create<MaterialVertexDeclaration>(decl);

    manager->AddAsset(ASSET_TYPE_VERTEXDECL, assetName, allocatedDecl);
    return true;
}
