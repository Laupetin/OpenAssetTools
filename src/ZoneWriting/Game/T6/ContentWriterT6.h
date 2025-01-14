#pragma once
#include "Game/T6/T6.h"
#include "Writing/ContentWriterBase.h"
#include "Writing/IContentWritingEntryPoint.h"

namespace T6
{
    class ContentWriter final : public ContentWriterBase, public IContentWritingEntryPoint
    {
    public:
        explicit ContentWriter(const Zone& zone);

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
} // namespace T6
