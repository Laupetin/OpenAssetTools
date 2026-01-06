#pragma once

#include "Game/IW4/IW4.h"
#include "Writing/ContentWriterBase.h"
#include "Writing/IContentWritingEntryPoint.h"

namespace IW4
{
    class ContentWriter final : public ContentWriterBase, public IContentWritingEntryPoint
    {
    public:
        explicit ContentWriter(const Zone& zone);

        void WriteContent(ZoneOutputStream& stream) override;

    private:
        void CreateXAssetList(XAssetList& xAssetList, MemoryManager& memory) const;

        void WriteScriptStringList(bool atStreamStart);

        void WriteXAsset(bool atStreamStart);
        void WriteXAssetArray(bool atStreamStart, size_t count);

        XAssetList* varXAssetList;
        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;

        ZoneOutputOffset varXAssetListWritten;
        ZoneOutputOffset varXAssetWritten;
        ZoneOutputOffset varScriptStringListWritten;
    };
} // namespace IW4
