#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "Game/IW3/IW3.h"

#include <map>
#include <string>
#include <vector>

namespace IW3
{
    class MenuConversionZoneState final : public IZoneAssetCreationState
    {
    public:
        std::map<std::string, std::vector<XAssetInfo<menuDef_t>*>> m_menus_by_filename;

        void AddLoadedFile(std::string loadedFileName, std::vector<XAssetInfo<menuDef_t>*> menusOfFile);
    };
} // namespace IW3
