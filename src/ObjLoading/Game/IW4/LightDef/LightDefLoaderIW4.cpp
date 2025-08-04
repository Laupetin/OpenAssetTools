#include "LightDefLoaderIW4.h"

#include "Game/IW4/IW4.h"
#include "LightDef/LightDefCommon.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace IW4;
using namespace ::light_def;

namespace
{
    constexpr auto MAX_IMAGE_NAME_SIZE = 0x800;

    class LoaderLightDef final : public AssetCreator<AssetLightDef>
    {
    public:
        LoaderLightDef(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto filename = GetFileNameForAsset(assetName);
            const auto file = m_search_path.Open(filename);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            const auto imageNameSize = file.m_length - sizeof(char) - sizeof(char);
            if (imageNameSize < 0 || imageNameSize > MAX_IMAGE_NAME_SIZE)
                return AssetCreationResult::Failure();

            auto* lightDef = m_memory.Alloc<GfxLightDef>();
            lightDef->name = m_memory.Dup(assetName.c_str());

            AssetRegistration<AssetLightDef> registration(assetName, lightDef);

            std::string imageName(static_cast<size_t>(imageNameSize), '\0');

            int8_t samplerState;
            int8_t lmapLookupStart;
            file.m_stream->read(reinterpret_cast<char*>(&samplerState), sizeof(int8_t));
            file.m_stream->read(&imageName[0], static_cast<size_t>(imageNameSize));
            file.m_stream->read(reinterpret_cast<char*>(&lmapLookupStart), sizeof(int8_t));

            auto* imageDependency = context.LoadDependency<AssetImage>(imageName);
            if (!imageDependency)
            {
                std::cerr << std::format("Could not load GfxLightDef \"{}\" due to missing image \"{}\"\n", assetName, imageName);
                return AssetCreationResult::Failure();
            }
            registration.AddDependency(imageDependency);

            lightDef->attenuation.samplerState = samplerState;
            lightDef->attenuation.image = imageDependency->Asset();
            lightDef->lmapLookupStart = static_cast<int>(static_cast<uint8_t>(lmapLookupStart));

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW4::light_def
{
    std::unique_ptr<AssetCreator<AssetLightDef>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<LoaderLightDef>(memory, searchPath);
    }
} // namespace IW4::light_def
