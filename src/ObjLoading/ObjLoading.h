#pragma once

#include "Zone/Zone.h"
#include "SearchPath/ISearchPath.h"

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
    static ISearchPath* GetIWDSearchPaths();

    static void LoadObjDataForZone(ISearchPath* searchPath, Zone* zone);
};
