#pragma once
#include "Loading/ContentLoaderBase.h"
#include "Loading/IContentLoadingEntryPoint.h"
#include "Game/IW5/IW5.h"

namespace IW5
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
