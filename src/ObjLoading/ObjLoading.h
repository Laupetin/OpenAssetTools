#pragma once

#include "Zone/Zone.h"
#include "SearchPath/ISearchPath.h"
#include "SearchPath/SearchPaths.h"

class ObjLoading
{
public:
    static class Configuration_t
    {
    public:
        bool Verbose = false;
    } Configuration;

    static void LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone);
    static void UnloadContainersOfZone(Zone* zone);

    static void LoadIWDsInSearchPath(ISearchPath* searchPath);
    static void UnloadIWDsInSearchPath(ISearchPath* searchPath);
    static SearchPaths GetIWDSearchPaths();

    static void LoadObjDataForZone(ISearchPath* searchPath, Zone* zone);
};
