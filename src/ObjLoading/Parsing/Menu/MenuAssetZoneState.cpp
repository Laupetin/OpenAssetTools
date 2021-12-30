#include "MenuAssetZoneState.h"

using namespace menu;

void MenuAssetZoneState::AddFunction(std::unique_ptr<CommonFunctionDef> function)
{
    m_functions_by_name.emplace(std::make_pair(function->m_name, function.get()));
    m_functions.emplace_back(std::move(function));
}

void MenuAssetZoneState::AddMenu(std::unique_ptr<CommonMenuDef> menu)
{
    m_menus.emplace_back(std::move(menu));
}
