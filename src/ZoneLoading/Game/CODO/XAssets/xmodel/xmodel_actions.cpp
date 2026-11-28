#include "xmodel_actions.h"

#include <cstring>

using namespace CODO;

Actions_XModel::Actions_XModel(Zone& zone)
    : AssetLoadingActions(zone)
{
}

void Actions_XModel::SetModelSurfs(XModelLodInfo* lodInfo, XModelSurfs* modelSurfs) const
{
    if (!modelSurfs)
        return;

    // The game copies these from the surfs asset into the lod once it is loaded
    static_assert(sizeof(lodInfo->partBits) + sizeof(lodInfo->unknown28) == sizeof(modelSurfs->partBits) + sizeof(unsigned int) * 2u);
    memcpy(lodInfo->partBits, modelSurfs->partBits, sizeof(lodInfo->partBits));
    memcpy(lodInfo->unknown28, modelSurfs->unknown24, sizeof(lodInfo->unknown28));
    lodInfo->surfs = modelSurfs->surfs;
}
