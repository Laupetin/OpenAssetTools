#pragma once

#include "Game/IW5/IW5.h"
#include "Loading/ContentLoaderBase.h"
#include "Loading/IContentLoadingEntryPoint.h"

namespace IW5
{
    class ContentLoader final : public ContentLoaderBase, public IContentLoadingEntryPoint
    {
    public:
        explicit ContentLoader(Zone& zone, ZoneInputStream& stream);
        void Load() override;

    private:
        void LoadScriptStringList(bool atStreamStart);

        void LoadXAsset(bool atStreamStart) const;
        void LoadXAssetArray(bool atStreamStart, size_t count);

        XAssetList* varXAssetList;
        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;
    };
} // namespace IW5
