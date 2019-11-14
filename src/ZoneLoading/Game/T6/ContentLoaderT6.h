#pragma once
#include "Loading/ContentLoader.h"
#include "Loading/IContentLoadingEntryPoint.h"
#include "Game/T6/T6.h"
#include "Loading/IZoneScriptStringProvider.h"

class ContentLoaderT6 final : public ContentLoader, public IContentLoadingEntryPoint, public IZoneScriptStringProvider
{
    std::vector<std::string> m_script_strings;
    T6::XAsset* varXAsset;
    T6::ScriptStringList* varScriptStringList;

    void LoadScriptStringList(bool atStreamStart);

    void LoadXAsset(bool atStreamStart);
    void LoadXAssetArray(bool atStreamStart, size_t count);

public:
    ContentLoaderT6();

    void Load(Zone* zone, IZoneInputStream* stream) override;
    std::string& GetZoneScriptString(scr_string_t scrString) override;
};
