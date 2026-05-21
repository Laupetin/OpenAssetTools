#include "gfximage_actions.h"

#include <cassert>
#include <cstring>

using namespace IW6;

Actions_GfxImage::Actions_GfxImage(Zone& zone)
    : AssetLoadingActions(zone)
{
}

void Actions_GfxImage::OnImageLoaded(GfxImage* image) const
{
    image->picmip.platform[0] = 0;
    image->picmip.platform[1] = 0;
}
