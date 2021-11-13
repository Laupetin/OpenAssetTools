#pragma once

#include "AssetLoading/IZoneAssetLoaderState.h"
#include "Domain/CommonFunctionDef.h"
#include "Domain/CommonMenuDef.h"

namespace menu
{
    class MenuAssetZoneState final : public IZoneAssetLoaderState
    {
    public:
        std::vector<std::unique_ptr<CommonFunctionDef>> m_functions;
        std::vector<std::unique_ptr<CommonMenuDef>> m_menus;

        MenuAssetZoneState() = default;
    };
}
