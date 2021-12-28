#pragma once

#include "AssetLoading/AssetLoadingContext.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/Zone.h"

class IObjLoader
{
public:
    virtual ~IObjLoader() = default;

    /**
     * \brief Checks whether this ObjLoader supports a specified zone.
     * \param zone The zone to check.
     * \return \c true if the specified zone is supported.
     */
    virtual bool SupportsZone(Zone* zone) const = 0;

    /**
     * \brief Loads all containers that are referenced by a specified zone.
     * \param searchPath The search path object to use to find the referenced containers.
     * \param zone The zone to check for referenced containers.
     */
    virtual void LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone) const = 0;

    /**
     * \brief Unloads all containers of a specified zone. If a container is also loaded by another zone it will only be unloaded when all referencing zones are unloaded.
     * \param zone The zone to unload all containers for.
     */
    virtual void UnloadContainersOfZone(Zone* zone) const = 0;

    /**
     * \brief Loads the obj data for all assets of a specified zone.
     * \param searchPath The search path object to use to find obj files.
     * \param zone The zone of the assets to load the obj data for.
     */
    virtual void LoadObjDataForZone(ISearchPath* searchPath, Zone* zone) const = 0;

    virtual bool LoadAssetForZone(AssetLoadingContext* context, asset_type_t assetType, const std::string& assetName) const = 0;
    virtual void FinalizeAssetsForZone(AssetLoadingContext* context) const = 0;
};