#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Asset/IAssetCreator.h"
using namespace T6;

#include "OBJ_Loader.h"

class CustomMapLinker
{
public:
    CustomMapLinker(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
    bool linkCustomMap(customMapInfo* projInfo);
};
