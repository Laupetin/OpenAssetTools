#include "clipmap_t_actions.h"

#include <cassert>
#include <cstring>

using namespace IW5;

Actions_clipMap_t::Actions_clipMap_t(Zone& zone)
    : AssetLoadingActions(zone)
{
}

void Actions_clipMap_t::ReallocClipInfo(const ClipInfo* clipInfo, clipMap_t* clipMap) const
{
    clipMap->pInfo = m_zone.GetMemory()->Alloc<ClipInfo>();
    memcpy(clipMap->pInfo, clipInfo, sizeof(ClipInfo));
}
