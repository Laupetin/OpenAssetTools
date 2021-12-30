#pragma once
#include "Loading/ContentLoaderBase.h"
#include "Loading/IContentLoadingEntryPoint.h"
#include "Game/T6/T6.h"

namespace T6
{
    class ContentLoader final : public ContentLoaderBase, public IContentLoadingEntryPoint
    {
        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;

        void LoadScriptStringList(bool atStreamStart);

        void LoadXAsset(bool atStreamStart) const;
        void LoadXAssetArray(bool atStreamStart, size_t count);

    public:
        ContentLoader();

        void Load(Zone* zone, IZoneInputStream* stream) override;
    };
}
