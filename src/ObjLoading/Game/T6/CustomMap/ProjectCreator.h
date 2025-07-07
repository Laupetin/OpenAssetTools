#pragma once

#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Game/T6/T6.h"
using namespace T6;

class CustomMapInfo
{
public:
    static customMapInfo* createCustomMapInfo(std::string& projectName, ISearchPath& searchPath);
};
