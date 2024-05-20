#include "SoundCurveLoader.h"

#include "Parsing/Graph2D/Graph2DReader.h"

#include <format>
#include <iostream>

namespace sound_curve
{
    std::unique_ptr<GenericGraph2D> LoadSoundCurve(const IAssetLoadingManager* manager, const std::string& soundCurveName)
    {
        auto* searchPath = manager->GetAssetLoadingContext()->m_raw_search_path;
        const auto fileName = std::format("soundaliases/{}.vfcurve", soundCurveName);
        const auto file = searchPath->Open(fileName);
        if (!file.IsOpen())
        {
            std::cerr << std::format("Failed to open file for sound curve \"{}\"\n", soundCurveName);
            return nullptr;
        }

        return graph2d::Read("sound curve", "SNDCURVE", *file.m_stream, fileName, soundCurveName);
    }
} // namespace sound_curve
