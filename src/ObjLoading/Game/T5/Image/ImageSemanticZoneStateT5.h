#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "Game/T5/T5.h"

#include <unordered_set>

namespace image
{
    class SemanticZoneStateT5 final : public IZoneAssetCreationState
    {
    public:
        SemanticZoneStateT5();

        void SetDelayLoadPixels(bool delayLoadPixels);
        [[nodiscard]] bool DelayLoadPixels() const;
        void TrackImage(const T5::GfxImage& image);
        void AssignSemantic(T5::GfxImage& image, unsigned char semantic);

    private:
        bool m_delay_load_pixels;
        std::unordered_set<const T5::GfxImage*> m_unassigned_images;
    };
} // namespace image
