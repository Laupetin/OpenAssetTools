#include "XModelDumperT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Json/JsonXModel.h"

#define GAME_NAMESPACE T6

#include "XModel/GenericXModelDumper.inc.h"

namespace T6
{
    void DumpXModel(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
    {
        DumpXModelJson(context, asset);
        DumpXModelSurfs(context, asset);
    }
} // namespace T6
