#pragma once

#include "IObjLoader.h"
#include "SearchPath/ISearchPath.h"
#include "Game/T6/T6.h"

class ObjLoaderT6 final : public IObjLoader
{
    static const int IPAK_READ_HASH;
    static const int GLOBAL_HASH;
    static int Com_HashKey(const char* str, int maxLen);

    static void LoadIPakForZone(ISearchPath* searchPath, const std::string& ipakName, Zone* zone);

    static void LoadImageFromIwiFile(T6::GfxImage* image, FileAPI::IFile* file, Zone* zone);
    static void LoadImageFromIwi(T6::GfxImage* image, ISearchPath* searchPath, Zone* zone);
    static void LoadImageFromLoadDef(T6::GfxImage* image, Zone* zone);
    static void LoadImageData(ISearchPath* searchPath, Zone* zone);

public:
    bool SupportsZone(Zone* zone) override;

    void LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone) override;
    void UnloadContainersOfZone(Zone* zone) override;

    void LoadObjDataForZone(ISearchPath* searchPath, Zone* zone) override;
};
