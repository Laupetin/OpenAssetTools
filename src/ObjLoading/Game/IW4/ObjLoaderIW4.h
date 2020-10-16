#pragma once

#include "IObjLoader.h"
#include "SearchPath/ISearchPath.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class ObjLoader final : public IObjLoader
    {
        static void LoadImageFromIwi(GfxImage* image, ISearchPath* searchPath, Zone* zone);
        static void LoadImageFromLoadDef(GfxImage* image, Zone* zone);
        static void LoadImageData(ISearchPath* searchPath, Zone* zone);

        static bool IsMpZone(Zone* zone);
        static bool IsZmZone(Zone* zone);

    public:
        bool SupportsZone(Zone* zone) const override;

        void LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone) const override;
        void UnloadContainersOfZone(Zone* zone) const override;

        void LoadObjDataForZone(ISearchPath* searchPath, Zone* zone) const override;
    };
}
