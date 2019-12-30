#include "ObjLoading.h"
#include "IObjLoader.h"
#include "Game/T6/ObjLoaderT6.h"
#include "ObjContainer/ObjContainerRegistry.h"

IObjLoader* objLoaders[]
{
    new ObjLoaderT6()
};

void ObjLoading::LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone)
{
    for (auto* loader : objLoaders)
    {
        if (loader->SupportsZone(zone))
        {
            loader->LoadReferencedContainersForZone(searchPath, zone);
            return;
        }
    }
}

void ObjLoading::LoadObjDataForZone(ISearchPath* searchPath, Zone* zone)
{
    for (auto* loader : objLoaders)
    {
        if (loader->SupportsZone(zone))
        {
            loader->LoadObjDataForZone(searchPath, zone);
            return;
        }
    }
}

void ObjLoading::UnloadContainersOfZone(Zone* zone)
{
    g_ObjContainerRegistry.RemoveContainerReferences(zone);
}
