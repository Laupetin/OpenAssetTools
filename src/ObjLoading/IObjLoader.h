#pragma once

#include "Asset/AssetCreatorCollection.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/Zone.h"

class IObjLoader
{
public:
    IObjLoader() = default;
    virtual ~IObjLoader() = default;
    IObjLoader(const IObjLoader& other) = default;
    IObjLoader(IObjLoader&& other) noexcept = default;
    IObjLoader& operator=(const IObjLoader& other) = default;
    IObjLoader& operator=(IObjLoader&& other) noexcept = default;

    /**
     * \brief Loads all containers that are referenced by a specified zone.
     * \param searchPath The search path object to use to find the referenced containers.
     * \param zone The zone to check for referenced containers.
     */
    virtual void LoadReferencedContainersForZone(ISearchPath& searchPath, Zone& zone) const = 0;

    /**
     * \brief Unloads all containers of a specified zone. If a container is also loaded by another zone it will only be unloaded when all referencing zones are
     * unloaded. \param zone The zone to unload all containers for.
     */
    virtual void UnloadContainersOfZone(Zone& zone) const = 0;

    virtual void ConfigureCreatorCollection(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath, IGdtQueryable& gdt) const = 0;

    static const IObjLoader* GetObjLoaderForGame(GameId game);
};
