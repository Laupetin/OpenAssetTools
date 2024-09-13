#include "XModelDumperT5.h"

#include "Game/T5/CommonT5.h"
#include "Game/T5/XModel/JsonXModel.h"

#define GAME_NAMESPACE T5

#include "XModel/GenericXModelDumper.inc.h"

namespace T5
{
    void DumpXModel(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
    {
        DumpXModelJson(context, asset);
        DumpXModelSurfs(context, asset);
    }
} // namespace T5
