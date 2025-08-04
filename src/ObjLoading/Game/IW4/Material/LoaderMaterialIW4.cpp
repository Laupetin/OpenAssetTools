#include "LoaderMaterialIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/Material/JsonMaterialLoaderIW4.h"
#include "Material/MaterialCommon.h"

#include <format>
#include <iostream>

using namespace IW4;
using namespace ::material;

namespace
{
    class MaterialLoader final : public AssetCreator<AssetMaterial>
    {
    public:
        MaterialLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(GetFileNameForAssetName(assetName));
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* material = m_memory.Alloc<Material>();
            material->info.name = m_memory.Dup(assetName.c_str());

            AssetRegistration<AssetMaterial> registration(assetName, material);
            if (!LoadMaterialAsJson(*file.m_stream, *material, m_memory, context, registration))
            {
                std::cerr << std::format("Failed to load material \"{}\"\n", assetName);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW4::material
{
    std::unique_ptr<AssetCreator<AssetMaterial>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<MaterialLoader>(memory, searchPath);
    }
} // namespace IW4::material
