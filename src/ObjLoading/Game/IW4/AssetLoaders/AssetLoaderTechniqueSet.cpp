#include "AssetLoaderTechniqueSet.h"

#include <cstring>
#include <sstream>
#include <type_traits>

#include "Utils/ClassUtils.h"
#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/TechsetConstantsIW4.h"
#include "Pool/GlobalAssetPool.h"
#include "Techset/TechsetFileReader.h"

using namespace IW4;

namespace IW4
{
    class TechniqueZoneLoadingState final : IZoneAssetLoaderState
    {
        std::map<std::string, MaterialTechnique*> m_loaded_techniques;

    public:
        _NODISCARD MaterialTechnique* FindLoadedTechnique(const std::string& techniqueName) const
        {
            const auto loadedTechnique = m_loaded_techniques.find(techniqueName);
            if (loadedTechnique != m_loaded_techniques.end())
                return loadedTechnique->second;

            return nullptr;
        }

        void AddLoadedTechnique(std::string techniqueName, MaterialTechnique* technique)
        {
            m_loaded_techniques.emplace(std::make_pair(std::move(techniqueName), technique));
        }
    };
}

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

MaterialTechnique* AssetLoaderTechniqueSet::LoadTechniqueFromRaw(const std::string& techniqueName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
{
    const auto techniqueFileName = GetTechniqueFileName(techniqueName);
    // TODO: Load technique or use previously loaded one
    return nullptr;
}

bool AssetLoaderTechniqueSet::CreateTechsetFromDefinition(const std::string& assetName, const techset::TechsetDefinition& definition, ISearchPath* searchPath, MemoryManager* memory,
                                                          IAssetLoadingManager* manager)
{
    auto* techset = memory->Create<MaterialTechniqueSet>();
    memset(techset, 0, sizeof(MaterialTechniqueSet));
    techset->name = memory->Dup(assetName.c_str());

    auto* techniqueZoneLoadingState = manager->GetAssetLoadingContext()->GetZoneAssetLoaderState<TechniqueZoneLoadingState>();
    for (auto i = 0u; i < std::extent_v<decltype(MaterialTechniqueSet::techniques)>; i++)
    {
        std::string techniqueName;
        if (definition.GetTechniqueByIndex(i, techniqueName))
        {
            auto* technique = techniqueZoneLoadingState->FindLoadedTechnique(techniqueName);
            if (!technique)
            {
                technique = LoadTechniqueFromRaw(techniqueName, searchPath, memory, manager);
                if (technique == nullptr)
                    return false;
                techniqueZoneLoadingState->AddLoadedTechnique(techniqueName, technique);
            }

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

    if (techsetDefinition)
        return CreateTechsetFromDefinition(assetName, *techsetDefinition, searchPath, memory, manager);

    return false;
}
