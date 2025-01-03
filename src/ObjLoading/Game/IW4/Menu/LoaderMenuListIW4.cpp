#include "LoaderMenuListIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/Menu/MenuConversionZoneStateIW4.h"
#include "Game/IW4/Menu/MenuConverterIW4.h"
#include "ObjLoading.h"
#include "Parsing/Menu/MenuFileReader.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace IW4;

namespace
{
    class MenuListLoader final : public AssetCreator<AssetMenuList>
    {
    public:
        MenuListLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            std::vector<menuDef_t*> menus;
            AssetRegistration<AssetMenuList> registration(assetName);

            auto& zoneState = context.GetZoneAssetCreationState<menu::MenuAssetZoneState>();
            auto& conversionState = context.GetZoneAssetCreationState<MenuConversionZoneState>();

            std::deque<std::string> menuLoadQueue;
            const auto alreadyLoadedMenuListFileMenus = conversionState.m_menus_by_filename.find(assetName);

            if (alreadyLoadedMenuListFileMenus == conversionState.m_menus_by_filename.end())
            {
                const auto file = m_search_path.Open(assetName);
                if (!file.IsOpen())
                    return AssetCreationResult::NoAction();

                const auto menuListResult = ParseMenuFile(*file.m_stream, assetName, zoneState);
                if (menuListResult)
                {
                    ProcessParsedResults(assetName, context, *menuListResult, zoneState, conversionState, menus, registration);

                    for (const auto& menuToLoad : menuListResult->m_menus_to_load)
                        menuLoadQueue.emplace_back(menuToLoad);

                    zoneState.AddMenusToLoad(assetName, std::move(menuListResult->m_menus_to_load));
                }
                else
                    return AssetCreationResult::Failure();
            }

            while (!menuLoadQueue.empty())
            {
                const auto& menuFileToLoad = menuLoadQueue.front();

                LoadMenuFileFromQueue(menuFileToLoad, context, zoneState, conversionState, menus, registration);

                menuLoadQueue.pop_front();
            }

            auto* menuListAsset = m_memory.Create<MenuList>();
            menuListAsset->name = m_memory.Dup(assetName.c_str());
            registration.SetAsset(menuListAsset);

            CreateMenuListAsset(*menuListAsset, menus);

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

        void FinalizeZone(AssetCreationContext& context) override
        {
            context.GetZoneAssetCreationState<MenuConversionZoneState>().FinalizeSupportingData();
        }

    private:
        bool LoadMenuFileFromQueue(const std::string& menuFilePath,
                                   AssetCreationContext& context,
                                   menu::MenuAssetZoneState& zoneState,
                                   MenuConversionZoneState& conversionState,
                                   std::vector<menuDef_t*>& menus,
                                   AssetRegistration<AssetMenuList>& registration)
        {
            const auto alreadyLoadedMenuFile = conversionState.m_menus_by_filename.find(menuFilePath);
            if (alreadyLoadedMenuFile != conversionState.m_menus_by_filename.end())
            {
                std::cout << std::format("Already loaded \"{}\", skipping\n", menuFilePath);
                for (auto* menu : alreadyLoadedMenuFile->second)
                {
                    menus.emplace_back(menu->Asset());
                    registration.AddDependency(menu);
                }
                return true;
            }

            const auto file = m_search_path.Open(menuFilePath);
            if (!file.IsOpen())
            {
                std::cerr << std::format("Could not open menu file \"{}\"\n", menuFilePath);
                return false;
            }

            const auto menuFileResult = ParseMenuFile(*file.m_stream, menuFilePath, zoneState);
            if (menuFileResult)
            {
                ProcessParsedResults(menuFilePath, context, *menuFileResult, zoneState, conversionState, menus, registration);
                if (!menuFileResult->m_menus_to_load.empty())
                    std::cout << std::format("WARNING: Menu file has menus to load even though it is not a menu list, ignoring: \"{}\"\n", menuFilePath);

                return true;
            }
            else
                std::cerr << std::format("Could not read menu file \"{}\"\n", menuFilePath);

            return false;
        }

        bool ProcessParsedResults(const std::string& fileName,
                                  AssetCreationContext& context,
                                  menu::ParsingResult& parsingResult,
                                  menu::MenuAssetZoneState& zoneState,
                                  MenuConversionZoneState& conversionState,
                                  std::vector<menuDef_t*>& menus,
                                  AssetRegistration<AssetMenuList>& registration)
        {
            const auto menuCount = parsingResult.m_menus.size();
            const auto functionCount = parsingResult.m_functions.size();
            const auto menuLoadCount = parsingResult.m_menus_to_load.size();
            auto totalItemCount = 0u;
            for (const auto& menu : parsingResult.m_menus)
                totalItemCount += menu->m_items.size();

            std::cout << std::format("Successfully read menu file \"{}\" ({} loads, {} menus, {} functions, {} items)\n",
                                     fileName,
                                     menuLoadCount,
                                     menuCount,
                                     functionCount,
                                     totalItemCount);

            // Add all functions to the zone state to make them available for all menus to be converted
            for (auto& function : parsingResult.m_functions)
                zoneState.AddFunction(std::move(function));

            // Prepare a list of all menus of this file
            std::vector<XAssetInfo<menuDef_t>*> allMenusOfFile;
            allMenusOfFile.reserve(parsingResult.m_menus.size());

            // Convert all menus and add them as assets
            for (auto& commonMenu : parsingResult.m_menus)
            {
                auto converter = IMenuConverter::Create(ObjLoading::Configuration.MenuNoOptimization, m_search_path, m_memory, context);

                auto* menuAsset = m_memory.Alloc<menuDef_t>();
                AssetRegistration<AssetMenu> menuRegistration(commonMenu->m_name, menuAsset);

                converter->ConvertMenu(*commonMenu, *menuAsset, menuRegistration);
                if (menuAsset == nullptr)
                {
                    std::cerr << std::format("Failed to convert menu file \"{}\"\n", commonMenu->m_name);
                    return false;
                }

                menus.emplace_back(menuAsset);
                auto* menuAssetInfo = context.AddAsset(std::move(menuRegistration));

                if (menuAssetInfo)
                {
                    allMenusOfFile.emplace_back(menuAssetInfo);
                    registration.AddDependency(menuAssetInfo);
                }

                zoneState.AddMenu(std::move(commonMenu));
            }

            // Register this file with all loaded menus
            conversionState.AddLoadedFile(fileName, std::move(allMenusOfFile));

            return true;
        }

        void CreateMenuListAsset(MenuList& menuList, const std::vector<menuDef_t*>& menus)
        {
            menuList.menuCount = static_cast<int>(menus.size());

            if (menuList.menuCount > 0)
            {
                menuList.menus = m_memory.Alloc<menuDef_t*>(menuList.menuCount);
                for (auto i = 0; i < menuList.menuCount; i++)
                    menuList.menus[i] = menus[i];
            }
            else
                menuList.menus = nullptr;
        }

        std::unique_ptr<menu::ParsingResult> ParseMenuFile(std::istream& stream, const std::string& menuFileName, const menu::MenuAssetZoneState& zoneState)
        {
            menu::MenuFileReader reader(stream, menuFileName, menu::FeatureLevel::IW4, m_search_path);

            reader.IncludeZoneState(zoneState);
            reader.SetPermissiveMode(ObjLoading::Configuration.MenuPermissiveParsing);

            return reader.ReadMenuFile();
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW4
{
    std::unique_ptr<AssetCreator<AssetMenuList>> CreateMenuListLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<MenuListLoader>(memory, searchPath);
    }
} // namespace IW4
