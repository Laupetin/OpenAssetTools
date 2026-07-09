#pragma once

#include "Game/QOS/QOS.h"
#include "Loading/ContentLoaderBase.h"
#include "Loading/IContentLoadingEntryPoint.h"

namespace QOS
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

        XAssetList* varXAssetList;
        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;
    };
} // namespace QOS
