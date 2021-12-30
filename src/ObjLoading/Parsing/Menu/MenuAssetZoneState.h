#pragma once

#include <string>

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

        std::map<std::string, CommonFunctionDef*> m_functions_by_name;

        MenuAssetZoneState() = default;
        
        void AddFunction(std::unique_ptr<CommonFunctionDef> function);
        void AddMenu(std::unique_ptr<CommonMenuDef> menu);
    };
}
