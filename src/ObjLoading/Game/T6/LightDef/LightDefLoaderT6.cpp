#include "LightDefLoaderT6.h"

#include "Game/T6/T6.h"
#include "LightDef/LightDefCommon.h"
#include "Utils/Logging/Log.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;

namespace
{
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
            const auto filename = light_def::GetFileNameForAsset(assetName);
            const auto file = m_search_path.Open(filename);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* lightDef = m_memory.Alloc<GfxLightDef>();
            lightDef->name = m_memory.Dup(assetName.c_str());

            AssetRegistration<AssetLightDef> registration(assetName, lightDef);

            std::string attenuationName;
            int8_t samplerState;
            int8_t lmapLookupStart;
            file.m_stream->read(reinterpret_cast<char*>(&samplerState), sizeof(int8_t));
            std::getline(*file.m_stream, attenuationName, '\0');

            auto* imageDependency = context.LoadDependency<AssetImage>(imageName);
            if (!imageDependency)
            {
                con::error("Could not load GfxLightDef \"{}\" due to missing image \"{}\"", assetName, imageName);
                return AssetCreationResult::Failure();
            }
            registration.AddDependency(imageDependency);

            lightDef->attenuation.samplerState = samplerState;
            lightDef->attenuation.image = imageDependency->Asset();
            lightDef->lmapLookupStart = 0;

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace light_def
{
    std::unique_ptr<AssetCreator<AssetLightDef>> CreateLoaderT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<LoaderLightDef>(memory, searchPath);
    }
} // namespace light_def
