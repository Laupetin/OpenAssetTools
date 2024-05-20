#pragma once

#include "AssetLoading/IAssetLoadingManager.h"
#include "Parsing/GenericGraph2D.h"

#include <memory>

namespace sound_curve
{
    std::unique_ptr<GenericGraph2D> LoadSoundCurve(const IAssetLoadingManager* manager, const std::string& soundCurveName);
}
