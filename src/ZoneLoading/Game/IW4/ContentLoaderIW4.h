#pragma once

#include "Game/IW4/IW4.h"
#include "Loading/ContentLoaderBase.h"
#include "Loading/IContentLoadingEntryPoint.h"

namespace IW4
{
    class ContentLoader final : public ContentLoaderBase, public IContentLoadingEntryPoint
    {
    public:
        ContentLoader();

        void Load(Zone* zone, IZoneInputStream* stream) override;

    private:
        void LoadScriptStringList(bool atStreamStart);

        void LoadXAsset(bool atStreamStart) const;
        void LoadXAssetArray(bool atStreamStart, size_t count);

        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;
    };
} // namespace IW4
