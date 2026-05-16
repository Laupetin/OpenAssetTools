#include "FxEffectDefCommon.h"

#include <format>

namespace fx_effect_def
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("fx/{}.efx", assetName);
    }
} // namespace fx_effect_def
