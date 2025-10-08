#include "SoundCurveCommon.h"

#include <format>

namespace sound_curve
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("soundaliases/{}.vfcurve", assetName);
    }
} // namespace sound_curve
