#include "xmodel_actions.h"

#include <cstring>

using namespace IW5;

Actions_XModel::Actions_XModel(Zone* zone)
    : AssetLoadingActions(zone)
{
}

void Actions_XModel::SetModelSurfs(XModelLodInfo* lodInfo, XModelSurfs* modelSurfs) const
{
    if (modelSurfs)
    {
        lodInfo->modelSurfs = m_zone->GetMemory()->Alloc<XModelSurfs>();
        memcpy(lodInfo->modelSurfs, modelSurfs, sizeof(XModelSurfs));
    }
}
