#include "SoundCurveCommon.h"

#include <format>

namespace snd_alias
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("soundaliases/{}.csv", assetName);
    }
} // namespace snd_alias
