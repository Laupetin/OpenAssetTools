#pragma once

#include <set>
#include <string>

#include "AssetLoading/IZoneAssetLoaderState.h"
#include "Domain/CommonFunctionDef.h"
#include "Domain/CommonMenuDef.h"

namespace menu
{
    class MenuAssetZoneState final : public IZoneAssetLoaderState
    {
    public:
        std::set<std::string> m_loaded_files;
        std::vector<std::unique_ptr<CommonFunctionDef>> m_functions;
        std::vector<std::unique_ptr<CommonMenuDef>> m_menus;

        MenuAssetZoneState() = default;
    };
}
