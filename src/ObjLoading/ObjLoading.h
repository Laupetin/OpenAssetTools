#pragma once

#include "AssetLoading/AssetLoadingContext.h"
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
        bool MenuPermissiveParsing = false;
        bool MenuNoOptimization = false;
    } Configuration;

    /**
     * \brief Loads all containers that are being referenced by the specified zone.
     * \param searchPath The search path to use to find the referenced containers.
     * \param zone The zone to load all referenced containers of.
     */
    static void LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone);

    /**
     * \brief Unloads all containers that were referenced by a specified zone. If referenced by more than one zone a container will only be unloaded once all referencing zones were unloaded the container.
     * \param zone The zone to unload all referenced containers for.
     */
    static void UnloadContainersOfZone(Zone* zone);

    /**
     * \brief Loads all IWDs that can be found in a specified search path.
     * \param searchPath The search path that contains IWDs to be loaded.
     */
    static void LoadIWDsInSearchPath(ISearchPath* searchPath);

    /**
     * \brief Unloads all IWDs that were loaded from the specified search path.
     * \param searchPath The search path that was used to load the IWDs to be unloaded.
     */
    static void UnloadIWDsInSearchPath(ISearchPath* searchPath);

    /**
     * \brief Creates a \c SearchPaths object containing all IWDs that are currently loaded.
     * \return A \c SearchPaths object containing all IWDs that are currently loaded.
     */
    static SearchPaths GetIWDSearchPaths();

    /**
     * \brief Loads the obj data for all assets of a zone.
     * \param searchPath The search path to use to search for all obj data files.
     * \param zone The zone of the assets to load the obj data for.
     */
    static void LoadObjDataForZone(ISearchPath* searchPath, Zone* zone);

    static bool LoadAssetForZone(AssetLoadingContext* context, asset_type_t assetType, const std::string& assetName);
    static void FinalizeAssetsForZone(AssetLoadingContext* context);
};
