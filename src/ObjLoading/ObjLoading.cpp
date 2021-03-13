#include "ObjLoading.h"

#include <fstream>

#include "IObjLoader.h"
#include "Game/IW4/ObjLoaderIW4.h"
#include "Game/T6/ObjLoaderT6.h"
#include "ObjContainer/IWD/IWD.h"
#include "SearchPath/SearchPaths.h"
#include "Utils/ObjFileStream.h"

ObjLoading::Configuration_t ObjLoading::Configuration;

const IObjLoader* const OBJ_LOADERS[]
{
    new IW4::ObjLoader(),
    new T6::ObjLoader()
};

void ObjLoading::LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone)
{
    for (const auto* loader : OBJ_LOADERS)
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
    for (const auto* loader : OBJ_LOADERS)
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
    for (const auto* loader : OBJ_LOADERS)
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
    searchPath->Find(SearchPathSearchOptions().IncludeSubdirectories(false).FilterExtensions("iwd"), [searchPath](const std::string& path)
    {
        auto file = std::make_unique<std::ifstream>(path, std::fstream::in | std::fstream::binary);

        if (file->is_open())
        {
            auto iwd = std::make_unique<IWD>(path, std::move(file));
            
            if (iwd->Initialize())
            {
                IWD::Repository.AddContainer(std::move(iwd), searchPath);
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

    for (auto* iwd : IWD::Repository)
    {
        iwdPaths.IncludeSearchPath(iwd);
    }

    return iwdPaths;
}

bool ObjLoading::LoadAssetForZone(AssetLoadingContext* context, const asset_type_t assetType, const std::string& assetName)
{
    for (const auto* loader : OBJ_LOADERS)
    {
        if (loader->SupportsZone(context->m_zone))
        {
            return loader->LoadAssetForZone(context, assetType, assetName);
        }
    }

    return false;
}
