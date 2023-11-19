#pragma once
#include "Game/T5/T5.h"
#include "Loading/ContentLoaderBase.h"
#include "Loading/IContentLoadingEntryPoint.h"

namespace T5
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
} // namespace T5
