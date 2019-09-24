#pragma once
#include "Loading/ContentLoader.h"
#include "Loading/IContentLoadingEntryPoint.h"
#include "Game/T6/T6.h"
#include "Loading/IZoneScriptStringProvider.h"

class ContentLoaderT6 final : public ContentLoader, public IContentLoadingEntryPoint, public IZoneScriptStringProvider
{
    std::vector<std::string> m_script_strings;

    void LoadScriptStringList(T6::ScriptStringList* scriptStringList);

    void LoadXAsset(T6::XAsset* pXAsset, bool atStreamStart);
    void LoadXAssetArray(T6::XAsset* pArray, size_t count, bool atStreamStart);

public:
    ContentLoaderT6();

    void Load(Zone* zone, IZoneInputStream* stream) override;
    std::string& GetZoneScriptString(scr_string_t scrString) override;
};
