#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "Game/IW3/IW3.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace IW3
{
    class MenuConversionZoneState final : public IZoneAssetCreationState
    {
    public:
        void AddLoadedFile(std::string loadedFileName, std::vector<XAssetInfo<menuDef_t>*> menusOfFile);

        std::unordered_map<std::string, std::vector<XAssetInfo<menuDef_t>*>> m_menus_by_filename;
    };
} // namespace IW3
