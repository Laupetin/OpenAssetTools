#pragma once

#include "IObjLoader.h"
#include "SearchPath/ISearchPath.h"

class ObjLoaderT6 final : public IObjLoader
{
    static const int IPAK_READ_HASH;
    static const int GLOBAL_HASH;
    static int Com_HashKey(const char* str, int maxLen);

    static void LoadIPakForZone(ISearchPath* searchPath, const std::string& ipakName, Zone* zone);

public:
    bool SupportsZone(Zone* zone) override;

    void LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone) override;
    void UnloadContainersOfZone(Zone* zone) override;

    void LoadObjDataForZone(ISearchPath* searchPath, Zone* zone) override;
};
