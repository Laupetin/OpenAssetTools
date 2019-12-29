#include "ObjLoading.h"
#include "IObjLoader.h"
#include "Game/T6/ObjLoaderT6.h"
#include "ObjContainer/ObjContainerRegistry.h"

IObjLoader* objLoaders[]
{
    new ObjLoaderT6()
};

void ObjLoading::LoadReferencedContainersForZone(Zone* zone)
{
    for (auto* loader : objLoaders)
    {
        if (loader->SupportsZone(zone))
        {
            loader->LoadReferencedContainersForZone(zone);
            return;
        }
    }
}

void ObjLoading::LoadObjDataForZone(Zone* zone)
{
    for (auto* loader : objLoaders)
    {
        if (loader->SupportsZone(zone))
        {
            loader->LoadObjDataForZone(zone);
            return;
        }
    }
}

void ObjLoading::UnloadContainersOfZone(Zone* zone)
{
    g_ObjContainerRegistry.RemoveContainerReferences(zone);
}
