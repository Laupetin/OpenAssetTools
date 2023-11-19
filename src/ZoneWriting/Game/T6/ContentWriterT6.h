#pragma once
#include "Game/T6/T6.h"
#include "Writing/ContentWriterBase.h"
#include "Writing/IContentWritingEntryPoint.h"

namespace T6
{
    class ContentWriter final : public ContentWriterBase, public IContentWritingEntryPoint
    {
        XAssetList* varXAssetList;
        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;

        void CreateXAssetList(XAssetList& xAssetList, MemoryManager& memory) const;

        void WriteScriptStringList(bool atStreamStart);

        void WriteXAsset(bool atStreamStart);
        void WriteXAssetArray(bool atStreamStart, size_t count);

    public:
        ContentWriter();

        void WriteContent(Zone* zone, IZoneOutputStream* stream) override;
    };
} // namespace T6
