#include "LoaderSoundCurveIW4.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Parsing/Graph2D/Graph2DReader.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <format>
#include <iostream>
#include <sstream>

using namespace IW4;

namespace
{
    class LoaderSoundCurve final : public AssetCreator<AssetSoundCurve>
    {
    public:
        LoaderSoundCurve(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = std::format("soundaliases/{}.vfcurve", assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            const auto sndCurveData = graph2d::Read("sound curve", "SNDCURVE", *file.m_stream, fileName, assetName);

            if (!sndCurveData)
                return AssetCreationResult::Failure();

            if (sndCurveData->knots.size() > std::extent_v<decltype(SndCurve::knots)>)
            {
                std::cerr << std::format("Failed to load SndCurve \"{}\": Too many knots ({})\n", assetName, sndCurveData->knots.size());
                return AssetCreationResult::Failure();
            }

            auto* sndCurve = m_memory.Alloc<SndCurve>();
            sndCurve->filename = m_memory.Dup(assetName.c_str());
            sndCurve->knotCount = static_cast<uint16_t>(sndCurveData->knots.size());

            for (auto i = 0u; i < std::extent_v<decltype(SndCurve::knots)>; i++)
            {
                if (i < sndCurveData->knots.size())
                {
                    const auto& [x, y] = sndCurveData->knots[i];
                    sndCurve->knots[i][0] = static_cast<float>(x);
                    sndCurve->knots[i][1] = static_cast<float>(y);
                }
                else
                {
                    sndCurve->knots[i][0] = 0;
                    sndCurve->knots[i][1] = 0;
                }
            }

            return AssetCreationResult::Success(context.AddAsset<AssetSoundCurve>(assetName, sndCurve));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW4
{
    std::unique_ptr<AssetCreator<AssetSoundCurve>> CreateSoundCurveLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<LoaderSoundCurve>(memory, searchPath);
    }
} // namespace IW4
