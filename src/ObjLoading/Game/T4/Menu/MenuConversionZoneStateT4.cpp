#include "MenuConversionZoneStateT4.h"

using namespace T4;

void MenuConversionZoneState::AddLoadedFile(std::string loadedFileName, std::vector<XAssetInfo<menuDef_t>*> menusOfFile)
{
    m_menus_by_filename.emplace(std::move(loadedFileName), std::move(menusOfFile));
}
