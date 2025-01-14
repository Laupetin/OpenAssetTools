#pragma once
#include "Game/T5/T5.h"
#include "Writing/ContentWriterBase.h"
#include "Writing/IContentWritingEntryPoint.h"

namespace T5
{
    class ContentWriter final : public ContentWriterBase, public IContentWritingEntryPoint
    {
    public:
        ContentWriter(const Zone& zone);

        void WriteContent(IZoneOutputStream& stream) override;

    private:
        void CreateXAssetList(XAssetList& xAssetList, MemoryManager& memory) const;

        void WriteScriptStringList(bool atStreamStart);

        void WriteXAsset(bool atStreamStart);
        void WriteXAssetArray(bool atStreamStart, size_t count);

        XAssetList* varXAssetList;
        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;
    };
} // namespace T5
