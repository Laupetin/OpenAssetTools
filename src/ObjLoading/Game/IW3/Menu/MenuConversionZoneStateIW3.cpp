#include "MenuConversionZoneStateIW3.h"

using namespace IW3;

void MenuConversionZoneState::AddLoadedFile(std::string loadedFileName, std::vector<XAssetInfo<menuDef_t>*> menusOfFile)
{
    m_menus_by_filename.emplace(std::move(loadedFileName), std::move(menusOfFile));
}
