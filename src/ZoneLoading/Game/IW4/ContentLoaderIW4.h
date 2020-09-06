#pragma once
#include "Loading/ContentLoader.h"
#include "Loading/IContentLoadingEntryPoint.h"
#include "Game/IW4/IW4.h"
#include "Loading/IZoneScriptStringProvider.h"

class ContentLoaderIW4 final : public ContentLoader, public IContentLoadingEntryPoint, public IZoneScriptStringProvider
{
    std::vector<std::string> m_script_strings;
    IW4::XAsset* varXAsset;
    IW4::ScriptStringList* varScriptStringList;

    void LoadScriptStringList(bool atStreamStart);

    void LoadXAsset(bool atStreamStart);
    void LoadXAssetArray(bool atStreamStart, size_t count);

public:
    ContentLoaderIW4();

    void Load(Zone* zone, IZoneInputStream* stream) override;
    std::string& GetZoneScriptString(scr_string_t scrString) override;
};
