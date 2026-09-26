#include "SpeakerMapCommon.h"

#include <format>

namespace speaker_map
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("soundaliases/{}.spkrmap", assetName);
    }
} // namespace speaker_map
