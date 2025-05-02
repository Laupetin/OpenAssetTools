#include "gfximage_actions.h"

#include <cassert>
#include <cstring>

using namespace T5;

Actions_GfxImage::Actions_GfxImage(Zone& zone)
    : AssetLoadingActions(zone)
{
}

void Actions_GfxImage::OnImageLoaded(GfxImage* image) const
{
    image->loadedSize = 0;
}

void Actions_GfxImage::LoadImageData(GfxImageLoadDef* loadDef, GfxImage* image) const
{
    const size_t loadDefSize = offsetof(GfxImageLoadDef, data) + loadDef->resourceSize;

    image->texture.loadDef = static_cast<GfxImageLoadDef*>(m_zone.GetMemory()->AllocRaw(loadDefSize));
    memcpy(image->texture.loadDef, loadDef, loadDefSize);
}
