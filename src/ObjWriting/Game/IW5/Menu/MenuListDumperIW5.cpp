#include "MenuListDumperIW5.h"

#include "Game/IW5/Menu/MenuWriterIW5.h"
#include "Menu/AbstractMenuWriter.h"
#include "MenuWriterIW5.h"
#include "ObjWriting.h"

#include <cassert>
#include <filesystem>
#include <format>
#include <set>
#include <sstream>

namespace fs = std::filesystem;

using namespace IW5;

namespace
{
    std::vector<const ExpressionSupportingData*> GetAllUniqueExpressionSupportingData(const MenuList* menuList)
    {
        std::vector<const ExpressionSupportingData*> result;
        std::set<const ExpressionSupportingData*> alreadyAddedSupportingData;

        if (menuList->menus == nullptr)
            return result;

        for (auto i = 0; i < menuList->menuCount; i++)
        {
            if (menuList->menus[i] == nullptr)
                continue;

            const auto* menu = menuList->menus[i];

            if (menu->data == nullptr || menu->data->expressionData == nullptr)
                continue;

            if (alreadyAddedSupportingData.find(menu->data->expressionData) == alreadyAddedSupportingData.end())
            {
                result.push_back(menu->data->expressionData);
                alreadyAddedSupportingData.emplace(menu->data->expressionData);
            }
        }

        return result;
    }

    void DumpFunctions(IW5::menu::IWriterIW5& menuDumper, const MenuList* menuList)
    {
        const auto allSupportingData = GetAllUniqueExpressionSupportingData(menuList);
        auto functionIndex = 0u;

        assert(allSupportingData.size() <= 1);

        for (const auto* supportingData : allSupportingData)
        {
            if (supportingData->uifunctions.functions == nullptr)
                continue;

            for (auto i = 0; i < supportingData->uifunctions.totalFunctions; i++)
            {
                const auto* function = supportingData->uifunctions.functions[i];
                if (function == nullptr)
                    continue;

                menuDumper.WriteFunctionDef(std::format("FUNC_{}", functionIndex), function);

                functionIndex++;
            }
        }
    }

    void DumpMenus(IW5::menu::IWriterIW5& menuDumper, const MenuList* menuList)
    {
        const fs::path p(menuList->name);

        std::string parentPath;
        if (p.has_parent_path())
            parentPath = p.parent_path().string() + "/";

        for (auto menuNum = 0; menuNum < menuList->menuCount; menuNum++)
        {
            const auto* menu = menuList->menus[menuNum];
            const auto* menuAssetName = menu->window.name;

            bool isReference = false;
            if (menuAssetName && menuAssetName[0] == ',')
            {
                menuAssetName = &menuAssetName[1];
                isReference = true;
            }

            std::ostringstream ss;
            ss << parentPath << menuAssetName << ".menu";

            const auto menuName = ss.str();

            // If the menu was embedded directly as menu list write its data in the menu list file
            if (!isReference && menuName == menuList->name)
                menuDumper.WriteMenu(*menu);
            else
                menuDumper.IncludeMenu(ss.str());
        }
    }
} // namespace

namespace IW5::menu
{
    bool MenuListDumper::ShouldDump(XAssetInfo<MenuList>* asset)
    {
        return true;
    }

    void MenuListDumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<MenuList>* asset)
    {
        const auto* menuList = asset->Asset();
        const auto assetFile = context.OpenAssetFile(asset->m_name);

        if (!assetFile)
            return;

        auto menuWriter = CreateMenuWriter(*assetFile);

        menuWriter->Start();

        if (!ObjWriting::Configuration.MenuLegacyMode)
            DumpFunctions(*menuWriter, menuList);

        DumpMenus(*menuWriter, menuList);

        menuWriter->End();
    }
} // namespace IW5::menu
