#include "MenuAssetZoneState.h"

#include "Utils/StringUtils.h"

using namespace menu;

void MenuAssetZoneState::AddFunction(std::unique_ptr<CommonFunctionDef> function)
{
    std::string lowerCaseFunctionName(function->m_name);
    utils::MakeStringLowerCase(lowerCaseFunctionName);

    m_functions_by_name.emplace(std::make_pair(lowerCaseFunctionName, function.get()));
    m_functions.emplace_back(std::move(function));
}

void MenuAssetZoneState::AddMenu(std::unique_ptr<CommonMenuDef> menu)
{
    m_menus.emplace_back(std::move(menu));
}

void MenuAssetZoneState::AddMenusToLoad(std::string menuName, std::vector<std::string> menusToLoad)
{
    m_menus_to_load_by_menu.emplace(std::make_pair(std::move(menuName), std::move(menusToLoad)));
}
