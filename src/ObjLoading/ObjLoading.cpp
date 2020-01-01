#include "ObjLoading.h"
#include "IObjLoader.h"
#include "Game/T6/ObjLoaderT6.h"
#include "ObjContainer/IWD/IWD.h"
#include "SearchPath/SearchPaths.h"

ObjLoading::Configuration_t ObjLoading::Configuration;

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
    for (auto* loader : objLoaders)
    {
        if (loader->SupportsZone(zone))
        {
            loader->UnloadContainersOfZone(zone);
            return;
        }
    }
}

void ObjLoading::LoadIWDsInSearchPath(ISearchPath* searchPath)
{
    searchPath->Find(SearchPathSearchOptions().IncludeSubdirectories(false).FilterExtensions("iwd"),
                     [searchPath](const std::string& path) -> void
                     {
                         auto file = FileAPI::Open(path, FileAPI::Mode::MODE_READ);

                         if (file.IsOpen())
                         {
                             const auto fileP = new FileAPI::File(std::move(file));
                             IWD* iwd = new IWD(path, fileP);

                             if (iwd->Initialize())
                             {
                                 IWD::Repository.AddContainer(iwd, searchPath);
                             }
                             else
                             {
                                 delete iwd;

                                 fileP->Close();
                                 delete fileP;
                             }
                         }
                     });
}

void ObjLoading::UnloadIWDsInSearchPath(ISearchPath* searchPath)
{
    IWD::Repository.RemoveContainerReferences(searchPath);
}

SearchPaths ObjLoading::GetIWDSearchPaths()
{
    SearchPaths iwdPaths;

    for(auto iwd : IWD::Repository)
    {
        iwdPaths.IncludeSearchPath(iwd);
    }

    return iwdPaths;
}
