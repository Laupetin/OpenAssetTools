#include "LoaderMaterialIW5.h"

#include "Game/IW5/IW5.h"
#include "JsonMaterialLoader.h"

#include <algorithm>
#include <cstring>
#include <format>
#include <iostream>

using namespace IW5;

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
            const auto file = m_search_path.Open(GetFileNameForAsset(assetName));
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
        std::string GetFileNameForAsset(const std::string& assetName)
        {
            std::string sanitizedFileName(assetName);
            if (sanitizedFileName[0] == '*')
            {
                std::ranges::replace(sanitizedFileName, '*', '_');
                const auto parenthesisPos = sanitizedFileName.find('(');
                if (parenthesisPos != std::string::npos)
                    sanitizedFileName.erase(parenthesisPos);
                sanitizedFileName = std::format("generated/{}", sanitizedFileName);
            }

            return std::format("materials/{}.json", sanitizedFileName);
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW5
{
    std::unique_ptr<AssetCreator<AssetMaterial>> CreateMaterialLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<MaterialLoader>(memory, searchPath);
    }
} // namespace IW5
