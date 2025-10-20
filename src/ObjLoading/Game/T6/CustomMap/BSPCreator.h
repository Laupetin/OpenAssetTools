#pragma once

#include "Game/T6/Maps/CustomMaps.h"
#include "SearchPath/ISearchPath.h"

class BSPCreator
{
public:
    static CustomMapBSP* createCustomMapBSP(std::string& mapName, ISearchPath& searchPath);
};
