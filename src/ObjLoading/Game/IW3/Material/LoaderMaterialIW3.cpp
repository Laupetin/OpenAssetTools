#include "LoaderMaterialIW3.h"

#include "Game/IW3/IW3.h"
#include "Game/IW3/Material/JsonMaterialLoaderIW3.h"
#include "Material/MaterialCommon.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <iostream>

using namespace IW3;

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
            const auto file = m_search_path.Open(material::GetFileNameForAssetName(assetName));
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* material = m_memory.Alloc<Material>();
            material->info.name = m_memory.Dup(assetName.c_str());

            AssetRegistration<AssetMaterial> registration(assetName, material);
            if (!LoadMaterialAsJson(*file.m_stream, *material, m_memory, context, registration))
            {
                con::error("Failed to load material \"{}\"", assetName);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace material
{
    std::unique_ptr<AssetCreator<AssetMaterial>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<MaterialLoader>(memory, searchPath);
    }
} // namespace material
