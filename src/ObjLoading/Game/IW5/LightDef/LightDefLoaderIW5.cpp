#include "LightDefLoaderIW5.h"

#include "Game/IW5/IW5.h"
#include "Game/IW5/LightDef/LightDefAssetCreationStateIW5.h"
#include "LightDef/LightDefCommon.h"
#include "Utils/Logging/Log.h"

#include <cstring>
#include <format>
#include <iostream>
#include <sstream>

using namespace IW5;

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

            int8_t attenuationSamplerState;
            file.m_stream->read(reinterpret_cast<char*>(&attenuationSamplerState), sizeof(int8_t));

            std::string attenuationName;
            std::getline(*file.m_stream, attenuationName, '\0');
            auto* attenuationImageDependency = context.LoadDependency<AssetImage>(attenuationName);
            if (!attenuationImageDependency)
            {
                con::error("Could not load GfxLightDef \"{}\" due to missing attenuation image \"{}\"", assetName, attenuationName);
                return AssetCreationResult::Failure();
            }
            registration.AddDependency(attenuationImageDependency);

            int8_t cucolorisSamplerState;
            file.m_stream->read(reinterpret_cast<char*>(&cucolorisSamplerState), sizeof(int8_t));

            std::string cucolorisName;
            std::getline(*file.m_stream, cucolorisName, '\0');
            if (!cucolorisName.empty())
            {
                auto* cucolorisImageDependency = context.LoadDependency<AssetImage>(cucolorisName);
                if (!cucolorisImageDependency)
                {
                    con::warn("Could not load GfxLightDef \"{}\" due to missing cucoloris image \"{}\"", assetName, cucolorisName);
                    return AssetCreationResult::Failure();
                }
                registration.AddDependency(cucolorisImageDependency);
                lightDef->cucoloris.image = cucolorisImageDependency->Asset();
            }
            lightDef->attenuation.samplerState = attenuationSamplerState;
            lightDef->attenuation.image = attenuationImageDependency->Asset();
            lightDef->cucoloris.samplerState = cucolorisSamplerState;

            context.GetZoneAssetCreationState<LightDefAssetCreationState>().SetLightDefLookupStart(lightDef, context);

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace light_def
{
    std::unique_ptr<AssetCreator<AssetLightDef>> CreateLoaderIW5(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<LoaderLightDef>(memory, searchPath);
    }
} // namespace light_def
