#include "gfximage_actions.h"

#include <cassert>
#include <cstring>

using namespace IW5;

Actions_GfxImage::Actions_GfxImage(Zone* zone)
    : AssetLoadingActions(zone)
{
}

void Actions_GfxImage::OnImageLoaded(GfxImage* image) const
{
    image->cardMemory.platform[0] = 0;
}

void Actions_GfxImage::LoadImageData(GfxImageLoadDef* loadDef, GfxImage* image) const
{
    const size_t loadDefSize = offsetof(IW5::GfxImageLoadDef, data) + loadDef->resourceSize;

    image->texture.loadDef = static_cast<GfxImageLoadDef*>(m_zone->GetMemory()->Alloc(loadDefSize));
    memcpy(image->texture.loadDef, loadDef, loadDefSize);
}
