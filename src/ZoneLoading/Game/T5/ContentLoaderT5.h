#pragma once

#include "Game/T5/T5.h"
#include "Loading/ContentLoaderBase.h"
#include "Loading/IContentLoadingEntryPoint.h"

namespace T5
{
    class ContentLoader final : public ContentLoaderBase, public IContentLoadingEntryPoint
    {
    public:
        explicit ContentLoader(Zone& zone);

        void Load(ZoneInputStream& stream) override;

    private:
        void LoadScriptStringList(bool atStreamStart);

        void LoadXAsset(bool atStreamStart) const;
        void LoadXAssetArray(bool atStreamStart, size_t count);

        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;
    };
} // namespace T5
