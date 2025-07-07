#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Game/T6/T6.h"
using namespace T6;

#include "OBJ_Loader.h"

class CustomMapInfo
{
public:
    static void parseMesh(objl::Mesh* OBJMesh, worldSurface* meshSurface, customMapInfo* projInfo);
    static customMapInfo* createCustomMapInfo(std::string& projectName, ISearchPath& searchPath);
};
