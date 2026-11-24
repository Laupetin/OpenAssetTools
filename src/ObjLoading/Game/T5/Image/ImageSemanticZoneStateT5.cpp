#include "ImageSemanticZoneStateT5.h"

using namespace T5;

namespace image
{
    SemanticZoneStateT5::SemanticZoneStateT5()
        : m_delay_load_pixels(true)
    {
    }

    void SemanticZoneStateT5::SetDelayLoadPixels(const bool delayLoadPixels)
    {
        m_delay_load_pixels = delayLoadPixels;
    }

    bool SemanticZoneStateT5::DelayLoadPixels() const
    {
        return m_delay_load_pixels;
    }

    void SemanticZoneStateT5::TrackImage(const GfxImage& image)
    {
        m_unassigned_images.emplace(&image);
    }

    void SemanticZoneStateT5::AssignSemantic(GfxImage& image, const unsigned char semantic)
    {
        if (m_unassigned_images.erase(&image) == 0)
            return;

        image.semantic = static_cast<char>(semantic);
        image.delayLoadPixels = m_delay_load_pixels && semantic != TS_2D;
    }
} // namespace image
