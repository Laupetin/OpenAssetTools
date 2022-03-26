#include "AssetLoaderTechniqueSet.h"

#include <cstring>
#include <sstream>
#include <type_traits>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/TechsetConstantsIW4.h"
#include "Pool/GlobalAssetPool.h"
#include "Techset/TechsetFileReader.h"

using namespace IW4;

void* AssetLoaderTechniqueSet::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* techset = memory->Create<MaterialTechniqueSet>();
    memset(techset, 0, sizeof(MaterialTechniqueSet));
    techset->name = memory->Dup(assetName.c_str());
    return techset;
}

std::string AssetLoaderTechniqueSet::GetTechniqueFileName(const std::string& techniqueName)
{
    std::ostringstream ss;
    ss << "techniques/" << techniqueName << ".tech";
    return ss.str();
}

std::string AssetLoaderTechniqueSet::GetTechsetFileName(const std::string& techsetAssetName)
{
    std::ostringstream ss;
    ss << "techsets/" << techsetAssetName << ".techset";
    return ss.str();
}

MaterialTechnique* AssetLoaderTechniqueSet::LoadTechniqueWithName(const std::string& techniqueName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
{
    // TODO: Load technique or use previously loaded one
    return nullptr;
}

bool AssetLoaderTechniqueSet::CreateTechsetFromDefinition(const std::string& assetName, const techset::TechsetDefinition& definition, ISearchPath* searchPath, MemoryManager* memory,
                                                          IAssetLoadingManager* manager)
{
    auto* techset = memory->Create<MaterialTechniqueSet>();
    memset(techset, 0, sizeof(MaterialTechniqueSet));
    techset->name = memory->Dup(assetName.c_str());

    for(auto i = 0u; i < std::extent_v<decltype(MaterialTechniqueSet::techniques)>; i++)
    {
        std::string techniqueName;
        if(definition.GetTechniqueByIndex(i, techniqueName))
        {
            auto* technique = LoadTechniqueWithName(techniqueName, searchPath, memory, manager);
            if (technique == nullptr)
                return false;
            techset->techniques[i] = technique;
        }
    }

    manager->AddAsset(ASSET_TYPE_TECHNIQUE_SET, assetName, techset);

    return true;
}

bool AssetLoaderTechniqueSet::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderTechniqueSet::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto techsetFileName = GetTechsetFileName(assetName);
    const auto file = searchPath->Open(techsetFileName);
    if (!file.IsOpen())
        return false;

    const TechsetFileReader reader(*file.m_stream, techsetFileName, techniqueTypeNames, std::extent_v<decltype(techniqueTypeNames)>);
    const auto techsetDefinition = reader.ReadTechsetDefinition();

    if(techsetDefinition)
        return CreateTechsetFromDefinition(assetName, *techsetDefinition, searchPath, memory, manager);

    return false;
}
