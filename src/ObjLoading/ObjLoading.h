#pragma once

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
     * \brief Loads all IWDs that can be found in a specified search path.
     * \param searchPath The search path that contains IWDs to be loaded.
     */
    static void LoadIWDsInSearchPath(ISearchPath& searchPath);

    /**
     * \brief Unloads all IWDs that were loaded from the specified search path.
     * \param searchPath The search path that was used to load the IWDs to be unloaded.
     */
    static void UnloadIWDsInSearchPath(ISearchPath& searchPath);

    /**
     * \brief Creates a \c SearchPaths object containing all IWDs that are currently loaded.
     * \return A \c SearchPaths object containing all IWDs that are currently loaded.
     */
    static SearchPaths GetIWDSearchPaths();
};
