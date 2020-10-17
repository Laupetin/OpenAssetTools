#pragma once
#include "Loading/ContentLoaderBase.h"
#include "Loading/IContentLoadingEntryPoint.h"
#include "Game/IW4/IW4.h"
#include "Loading/IZoneScriptStringProvider.h"

namespace IW4
{
    class ContentLoader final : public ContentLoaderBase, public IContentLoadingEntryPoint, public IZoneScriptStringProvider
    {
        std::vector<std::string> m_script_strings;
        XAsset* varXAsset;
        ScriptStringList* varScriptStringList;

        void LoadScriptStringList(bool atStreamStart);

        void LoadXAsset(bool atStreamStart);
        void LoadXAssetArray(bool atStreamStart, size_t count);

    public:
        ContentLoader();

        void Load(Zone* zone, IZoneInputStream* stream) override;
        std::string& GetZoneScriptString(scr_string_t scrString) override;
    };
}
