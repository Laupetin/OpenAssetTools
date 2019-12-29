#pragma once

#include "IObjLoader.h"

class ObjLoaderT6 final : public IObjLoader
{
    static const int IPAK_READ_HASH;
    static const int GLOBAL_HASH;
    static int Com_HashKey(const char* str, int maxLen);

    static void LoadIPakForZone(std::string ipakName, Zone* zone);

public:
    bool SupportsZone(Zone* zone) override;
    void LoadReferencedContainersForZone(Zone* zone) override;
    void LoadObjDataForZone(Zone* zone) override;
};
