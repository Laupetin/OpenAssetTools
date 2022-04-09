#include "AssetLoaderTechniqueSet.h"

#include <cstring>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <type_traits>

#include "Utils/ClassUtils.h"
#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/TechsetConstantsIW4.h"
#include "Pool/GlobalAssetPool.h"
#include "Techset/TechniqueFileReader.h"
#include "Techset/TechsetFileReader.h"

using namespace IW4;

namespace IW4
{
    class TechniqueCreator final : public techset::ITechniqueDefinitionAcceptor
    {
        IAssetLoadingManager* const m_manager;

    public:
        struct Pass
        {
            XAssetInfo<MaterialVertexShader>* m_vertex_shader;
            XAssetInfo<MaterialPixelShader>* m_pixel_shader;
            MaterialVertexDeclaration m_vertex_decl;
            std::vector<MaterialShaderArgument> m_arguments;

            Pass()
                : m_vertex_shader(nullptr),
                  m_pixel_shader(nullptr),
                  m_vertex_decl{}
            {
            }
        };

        std::vector<Pass> m_passes;
        std::vector<XAssetInfoGeneric*> m_dependencies;

        explicit TechniqueCreator(IAssetLoadingManager* manager)
            : m_manager(manager)
        {
            m_passes.emplace_back();
        }

        void AcceptNextPass() override
        {
            m_passes.emplace_back();
        }

        void AcceptStateMap(const std::string& stateMapName) override
        {
            // TODO: State maps currently are not used
        }

        bool AcceptVertexShader(const std::string& vertexShaderName, std::string& errorMessage) override
        {
            auto* vertexShaderDependency = m_manager->LoadDependency(ASSET_TYPE_VERTEXSHADER, vertexShaderName);
            if (vertexShaderDependency == nullptr)
            {
                errorMessage = "Failed to load specified shader";
                return false;
            }

            auto& pass = m_passes.at(m_passes.size() - 1);
            pass.m_vertex_shader = reinterpret_cast<XAssetInfo<MaterialVertexShader>*>(vertexShaderDependency);

            return true;
        }

        bool AcceptPixelShader(const std::string& pixelShaderName, std::string& errorMessage) override
        {
            auto* pixelShaderDependency = m_manager->LoadDependency(ASSET_TYPE_PIXELSHADER, pixelShaderName);
            if (pixelShaderDependency == nullptr)
            {
                errorMessage = "Failed to load specified shader";
                return false;
            }

            auto& pass = m_passes.at(m_passes.size() - 1);
            pass.m_pixel_shader = reinterpret_cast<XAssetInfo<MaterialPixelShader>*>(pixelShaderDependency);

            return true;
        }

        bool AcceptShaderCodeArgument(techset::ShaderSelector shader, techset::ShaderArgument shaderArgument, techset::ShaderArgumentCodeSource source, std::string& errorMessage) override
        {
            return true;
        }

        bool AcceptShaderLiteralArgument(techset::ShaderSelector shader, techset::ShaderArgument shaderArgument, techset::ShaderArgumentLiteralSource source, std::string& errorMessage) override
        {
            return true;
        }

        bool AcceptShaderMaterialArgument(techset::ShaderSelector shader, techset::ShaderArgument shaderArgument, techset::ShaderArgumentMaterialSource source, std::string& errorMessage) override
        {
            return true;
        }

        bool AcceptVertexStreamRouting(const std::string& destination, const std::string& source, std::string& errorMessage) override
        {
            return true;
        }
    };

    class LoadedTechnique
    {
    public:
        MaterialTechnique* m_technique;
        std::vector<XAssetInfoGeneric*> m_dependencies;

        LoadedTechnique(MaterialTechnique* technique, std::vector<XAssetInfoGeneric*> dependencies)
            : m_technique(technique),
              m_dependencies(std::move(dependencies))
        {
        }
    };

    class TechniqueZoneLoadingState final : public IZoneAssetLoaderState
    {
        std::unordered_map<std::string, std::unique_ptr<LoadedTechnique>> m_loaded_techniques;

    public:
        _NODISCARD const LoadedTechnique* FindLoadedTechnique(const std::string& techniqueName) const
        {
            const auto loadedTechnique = m_loaded_techniques.find(techniqueName);
            if (loadedTechnique != m_loaded_techniques.end())
                return loadedTechnique->second.get();

            return nullptr;
        }

        const LoadedTechnique* AddLoadedTechnique(std::string techniqueName, MaterialTechnique* technique, std::vector<XAssetInfoGeneric*> dependencies)
        {
            return m_loaded_techniques.emplace(std::make_pair(std::move(techniqueName), std::make_unique<LoadedTechnique>(technique, std::move(dependencies)))).first->second.get();
        }
    };

    class TechniqueLoader
    {
        ISearchPath* m_search_path;
        MemoryManager* m_memory;
        IAssetLoadingManager* m_manager;
        TechniqueZoneLoadingState* m_zone_state;

        static std::string GetTechniqueFileName(const std::string& techniqueName)
        {
            std::ostringstream ss;
            ss << "techniques/" << techniqueName << ".tech";
            return ss.str();
        }

        MaterialTechnique* LoadTechniqueFromRaw(const std::string& techniqueName, std::vector<XAssetInfoGeneric*>& dependencies)
        {
            const auto techniqueFileName = GetTechniqueFileName(techniqueName);
            const auto file = m_search_path->Open(techniqueFileName);
            if (!file.IsOpen())
                return nullptr;

            TechniqueCreator creator(m_manager);
            techset::TechniqueFileReader reader(*file.m_stream, techniqueFileName, &creator);
            if (!reader.ReadTechniqueDefinition())
                return nullptr;
            // TODO: Load technique or use previously loaded one
            return nullptr;
        }

    public:
        TechniqueLoader(ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
            : m_search_path(searchPath),
              m_memory(memory),
              m_manager(manager),
              m_zone_state(manager->GetAssetLoadingContext()->GetZoneAssetLoaderState<TechniqueZoneLoadingState>())
        {
        }

        const LoadedTechnique* LoadMaterialTechnique(const std::string& techniqueName)
        {
            auto* technique = m_zone_state->FindLoadedTechnique(techniqueName);
            if (technique)
                return technique;

            std::vector<XAssetInfoGeneric*> dependencies;
            auto* techniqueFromRaw = LoadTechniqueFromRaw(techniqueName, dependencies);
            if (technique == nullptr)
                return nullptr;

            return m_zone_state->AddLoadedTechnique(techniqueName, techniqueFromRaw, dependencies);
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

std::string AssetLoaderTechniqueSet::GetTechsetFileName(const std::string& techsetAssetName)
{
    std::ostringstream ss;
    ss << "techsets/" << techsetAssetName << ".techset";
    return ss.str();
}

bool AssetLoaderTechniqueSet::CreateTechsetFromDefinition(const std::string& assetName, const techset::TechsetDefinition& definition, ISearchPath* searchPath, MemoryManager* memory,
                                                          IAssetLoadingManager* manager)
{
    auto* techset = memory->Create<MaterialTechniqueSet>();
    memset(techset, 0, sizeof(MaterialTechniqueSet));
    techset->name = memory->Dup(assetName.c_str());

    TechniqueLoader techniqueLoader(searchPath, memory, manager);
    std::set<XAssetInfoGeneric*> dependencies;
    for (auto i = 0u; i < std::extent_v<decltype(MaterialTechniqueSet::techniques)>; i++)
    {
        std::string techniqueName;
        if (definition.GetTechniqueByIndex(i, techniqueName))
        {
            auto* technique = techniqueLoader.LoadMaterialTechnique(techniqueName);

            if (!technique)
                return false;

            for (auto* techniqueDependency : technique->m_dependencies)
                dependencies.emplace(techniqueDependency);

            techset->techniques[i] = technique->m_technique;
        }
    }

    manager->AddAsset(ASSET_TYPE_TECHNIQUE_SET, assetName, techset, std::vector(dependencies.begin(), dependencies.end()), std::vector<scr_string_t>());

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

    const techset::TechsetFileReader reader(*file.m_stream, techsetFileName, techniqueTypeNames, std::extent_v<decltype(techniqueTypeNames)>);
    const auto techsetDefinition = reader.ReadTechsetDefinition();

    if (techsetDefinition)
        return CreateTechsetFromDefinition(assetName, *techsetDefinition, searchPath, memory, manager);

    return false;
}
