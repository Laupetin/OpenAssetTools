#include "LoadedSoundCommon.h"

#include <format>

namespace loaded_sound
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("sound/{}.wav", assetName);
    }
} // namespace loaded_sound
