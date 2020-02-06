#include "gfximage_actions.h"
#include <cassert>

Actions_GfxImage::Actions_GfxImage(Zone* zone)
    : AssetLoadingActions(zone)
{
}

void Actions_GfxImage::OnImageLoaded(T6::GfxImage* image) const
{
    image->loadedSize = 0;
}

void Actions_GfxImage::LoadImageData(T6::GfxImageLoadDef* loadDef, T6::GfxImage* image) const
{
    if(loadDef->resourceSize > 0)
    {
        const size_t loadDefSize = offsetof(T6::GfxImageLoadDef, data) + loadDef->resourceSize;

        image->texture.loadDef = static_cast<T6::GfxImageLoadDef*>(m_zone->GetMemory()->Alloc(loadDefSize));
        memcpy(image->texture.loadDef, loadDef, loadDefSize);
    }
    else
    {
        image->texture.loadDef = nullptr;
    }
}
