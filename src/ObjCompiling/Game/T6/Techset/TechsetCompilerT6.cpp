#include "TechsetCompilerT6.h"

#include "Game/T6/T6.h"
#include "Game/T6/Techset/TechsetConstantsT6.h"
#include "Techset/CommonTechniqueLoader.h"
#include "Techset/CommonTechsetLoader.h"
#include "Techset/TechniqueCache.h"
#include "Techset/TechsetCommon.h"

#include <cassert>

using namespace T6;

namespace
{
    MaterialWorldVertexFormat GetWorldVertexFormat(const std::string& name)
    {
        if (name.contains("lit_"))
        {
            size_t texCount = 0u, normalCount = 0u;
            techset::CountWorldVertFormatParameters(name, texCount, normalCount);

            // 0 and 1 seem to be treated equally
            texCount = std::max(texCount, 1u);
            normalCount = std::max(normalCount, 1u);

            if (texCount == 1 && normalCount == 1)
                return MTL_WORLDVERT_TEX_1_NRM_1;
            if (texCount == 2 && normalCount == 1)
                return MTL_WORLDVERT_TEX_2_NRM_1;
            if (texCount == 2 && normalCount == 2)
                return MTL_WORLDVERT_TEX_2_NRM_2;
            if (texCount == 3 && normalCount == 1)
                return MTL_WORLDVERT_TEX_3_NRM_1;
            if (texCount == 3 && normalCount == 2)
                return MTL_WORLDVERT_TEX_3_NRM_2;
            if (texCount == 3 && normalCount == 3)
                return MTL_WORLDVERT_TEX_3_NRM_3;
            if (texCount == 4 && normalCount == 1)
                return MTL_WORLDVERT_TEX_4_NRM_1;
            if (texCount == 4 && normalCount == 2)
                return MTL_WORLDVERT_TEX_4_NRM_2;
            if (texCount == 4 && normalCount == 3)
                return MTL_WORLDVERT_TEX_4_NRM_3;
        }

        return static_cast<MaterialWorldVertexFormat>(0);
    }

    MaterialTechniqueSet* ConvertTechniqueSet(const techset::CommonTechset& commonTechset, MemoryManager& memory)
    {
        auto* techset = memory.Alloc<MaterialTechniqueSet>();
        techset->name = memory.Dup(commonTechset.m_name.c_str());
        techset->worldVertFormat = GetWorldVertexFormat(commonTechset.m_name);

        return techset;
    }

    MaterialTechnique* ConvertTechnique(const techset::CommonTechnique& commonTechnique, MemoryManager& memory)
    {
        const auto additionalPassCount = std::max(commonTechnique.m_passes.size(), 1u) - 1u;
        auto* technique = static_cast<MaterialTechnique*>(memory.AllocRaw(sizeof(MaterialTechnique) + additionalPassCount * sizeof(MaterialPass)));

        technique->name = memory.Dup(commonTechnique.m_name.c_str());
        technique->passCount = static_cast<uint16_t>(commonTechnique.m_passes.size());

        return technique;
    }

    MaterialTechnique* LoadAndConvertTechnique(const std::string& techniqueName, techset::TechniqueCache& cache, MemoryManager& memory, ISearchPath& searchPath)
    {
        const auto commonTechnique = techset::LoadCommonTechnique(techniqueName, commonCodeSourceInfos, commonRoutingInfos, searchPath);
        if (!commonTechnique)
            return nullptr;

        auto* convertedTechnique = ConvertTechnique(*commonTechnique, memory);
        assert(convertedTechnique);
        cache.AddTechniqueToCache(techniqueName, convertedTechnique);

        return convertedTechnique;
    }

    class TechsetCompilerT6 final : public AssetCreator<AssetTechniqueSet>
    {
    public:
        TechsetCompilerT6(ISearchPath& searchPath, MemoryManager& memory)
            : m_search_path(searchPath),
              m_memory(memory)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            bool failure = false;
            const auto commonTechset = techset::LoadCommonTechset(assetName, commonTechniqueTypeNames, m_search_path, failure);
            if (!commonTechset)
                return failure ? AssetCreationResult::Failure() : AssetCreationResult::NoAction();

            auto* techset = ConvertTechniqueSet(*commonTechset, m_memory);
            auto& techniqueCache = context.GetZoneAssetCreationState<techset::TechniqueCache>();

            for (auto techniqueIndex = 0u; techniqueIndex < std::extent_v<decltype(MaterialTechniqueSet::techniques)>; techniqueIndex++)
            {
                const auto& techniqueName = commonTechset->m_technique_names[techniqueIndex];
                if (techniqueName.empty())
                    continue;

                auto* technique = techniqueCache.GetCachedTechnique<MaterialTechnique>(techniqueName);
                if (!technique)
                    technique = LoadAndConvertTechnique(techniqueName, techniqueCache, m_memory, m_search_path);

                if (!technique)
                    return AssetCreationResult::Failure();

                techset->techniques[techniqueIndex] = technique;
            }

            return AssetCreationResult::Success(context.AddAsset(AssetRegistration<AssetTechniqueSet>(assetName, techset)));
        }

    private:
        ISearchPath& m_search_path;
        MemoryManager& m_memory;
    };
} // namespace

namespace techset
{
    std::unique_ptr<IAssetCreator> CreateCompilerT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<TechsetCompilerT6>(searchPath, memory);
    }
} // namespace techset
