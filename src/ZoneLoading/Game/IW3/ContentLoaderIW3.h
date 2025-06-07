#pragma once

#include "Game/IW3/IW3.h"
#include "Loading/ContentLoaderBase.h"
#include "Loading/IContentLoadingEntryPoint.h"

namespace IW3
{
    class ContentLoader final : public ContentLoaderBase, public IContentLoadingEntryPoint
    {
    public:
        ContentLoader(Zone& zone, ZoneInputStream& stream);
        void Load() override;

    private:
        void LoadScriptStringList(bool atStreamStart);

        void LoadXAsset(bool atStreamStart) const;
        void LoadXAssetArray(bool atStreamStart, size_t count);

        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;
    };
} // namespace IW3
