#pragma once

#include "Game/IW5/IW5.h"
#include "Loading/ContentLoaderBase.h"
#include "Loading/IContentLoadingEntryPoint.h"

namespace IW5
{
    class ContentLoader final : public ContentLoaderBase, public IContentLoadingEntryPoint
    {
    public:
        explicit ContentLoader(Zone& zone);

        void Load(IZoneInputStream* stream) override;

    private:
        void LoadScriptStringList(bool atStreamStart);

        void LoadXAsset(bool atStreamStart) const;
        void LoadXAssetArray(bool atStreamStart, size_t count);

        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;
    };
} // namespace IW5
