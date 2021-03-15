#pragma once
#include "Writing/ContentWriterBase.h"
#include "Writing/IContentWritingEntryPoint.h"
#include "Game/T6/T6.h"

namespace T6
{
    class ContentWriter final : public ContentWriterBase, public IContentWritingEntryPoint
    {
        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;

        void LoadScriptStringList(bool atStreamStart);

        void LoadXAsset(bool atStreamStart);
        void LoadXAssetArray(bool atStreamStart, size_t count);

    public:
        ContentWriter();

        void WriteContent(Zone* zone, IZoneOutputStream* stream) override;
    };
}
