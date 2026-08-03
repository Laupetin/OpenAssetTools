#include "LoaderMenuListIW3.h"

#include "Game/IW3/Menu/MenuConversionZoneStateIW3.h"
#include "Game/IW3/Menu/MenuConverterIW3.h"
#include "ObjLoading.h"
#include "Parsing/Menu/MenuAssetZoneState.h"
#include "Parsing/Menu/MenuFileReader.h"
#include "Utils/Logging/Log.h"

#include <deque>

using namespace IW3;

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
            auto& parsingState = context.GetZoneAssetCreationState<menu::MenuAssetZoneState>();
            auto& conversionState = context.GetZoneAssetCreationState<MenuConversionZoneState>();
            std::deque<std::string> menuLoadQueue;

            const auto alreadyLoaded = conversionState.m_menus_by_filename.find(assetName);
            if (alreadyLoaded != conversionState.m_menus_by_filename.end())
            {
                AddExistingMenus(alreadyLoaded->second, menus, registration);
            }
            else
            {
                const auto file = m_search_path.Open(assetName);
                if (!file.IsOpen())
                    return AssetCreationResult::NoAction();

                auto result = ParseMenuFile(*file.m_stream, assetName, parsingState);
                if (!result || !ProcessParsedResults(assetName, context, *result, parsingState, conversionState, menus, registration))
                    return AssetCreationResult::Failure();

                for (const auto& menuToLoad : result->m_menus_to_load)
                    menuLoadQueue.emplace_back(menuToLoad);
                parsingState.AddMenusToLoad(assetName, std::move(result->m_menus_to_load));
            }

            while (!menuLoadQueue.empty())
            {
                if (!LoadMenuFile(menuLoadQueue.front(), context, parsingState, conversionState, menus, registration))
                    return AssetCreationResult::Failure();
                menuLoadQueue.pop_front();
            }

            auto* asset = m_memory.Alloc<MenuList>();
            asset->name = m_memory.Dup(assetName.c_str());
            asset->menuCount = static_cast<int>(menus.size());
            if (!menus.empty())
            {
                asset->menus = m_memory.Alloc<menuDef_t*>(menus.size());
                for (auto i = 0u; i < menus.size(); i++)
                    asset->menus[i] = menus[i];
            }

            registration.SetAsset(asset);
            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        static void AddExistingMenus(const std::vector<XAssetInfo<menuDef_t>*>& existing,
                                     std::vector<menuDef_t*>& menus,
                                     AssetRegistration<AssetMenuList>& registration)
        {
            for (auto* menu : existing)
            {
                menus.emplace_back(menu->Asset());
                registration.AddDependency(menu);
            }
        }

        bool LoadMenuFile(const std::string& fileName,
                          AssetCreationContext& context,
                          menu::MenuAssetZoneState& parsingState,
                          MenuConversionZoneState& conversionState,
                          std::vector<menuDef_t*>& menus,
                          AssetRegistration<AssetMenuList>& registration) const
        {
            const auto alreadyLoaded = conversionState.m_menus_by_filename.find(fileName);
            if (alreadyLoaded != conversionState.m_menus_by_filename.end())
            {
                con::debug("Already loaded \"{}\", skipping", fileName);
                AddExistingMenus(alreadyLoaded->second, menus, registration);
                return true;
            }

            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
            {
                con::error("Could not open menu file \"{}\"", fileName);
                return false;
            }

            auto result = ParseMenuFile(*file.m_stream, fileName, parsingState);
            if (!result)
            {
                con::error("Could not read menu file \"{}\"", fileName);
                return false;
            }
            if (!result->m_menus_to_load.empty())
                con::warn("Menu file has menus to load even though it is not a menu list, ignoring: \"{}\"", fileName);

            return ProcessParsedResults(fileName, context, *result, parsingState, conversionState, menus, registration);
        }

        bool ProcessParsedResults(const std::string& fileName,
                                  AssetCreationContext& context,
                                  menu::ParsingResult& result,
                                  menu::MenuAssetZoneState& parsingState,
                                  MenuConversionZoneState& conversionState,
                                  std::vector<menuDef_t*>& menus,
                                  AssetRegistration<AssetMenuList>& registration) const
        {
            auto itemCount = 0uz;
            for (const auto& menu : result.m_menus)
                itemCount += menu->m_items.size();

            con::info(
                "Successfully read menu file \"{}\" ({} loads, {} menus, {} items)", fileName, result.m_menus_to_load.size(), result.m_menus.size(), itemCount);

            std::vector<XAssetInfo<menuDef_t>*> menusOfFile;
            menusOfFile.reserve(result.m_menus.size());
            for (auto& commonMenu : result.m_menus)
            {
                auto converter = IMenuConverter::Create(ObjLoading::Configuration.MenuNoOptimization, m_search_path, m_memory, context);
                auto* menuAsset = m_memory.Alloc<menuDef_t>();
                AssetRegistration<AssetMenu> menuRegistration(commonMenu->m_name, menuAsset);
                if (!converter->ConvertMenu(*commonMenu, *menuAsset, menuRegistration))
                    return false;

                menus.emplace_back(menuAsset);
                auto* menuInfo = context.AddAsset(std::move(menuRegistration));
                if (menuInfo)
                {
                    menusOfFile.emplace_back(menuInfo);
                    registration.AddDependency(menuInfo);
                }
                parsingState.AddMenu(std::move(commonMenu));
            }

            conversionState.AddLoadedFile(fileName, std::move(menusOfFile));
            return true;
        }

        std::unique_ptr<menu::ParsingResult>
            ParseMenuFile(std::istream& stream, const std::string& fileName, const menu::MenuAssetZoneState& parsingState) const
        {
            menu::MenuFileReader reader(stream, fileName, menu::FeatureLevel::IW3, m_search_path);
            reader.IncludeZoneState(parsingState);
            reader.SetPermissiveMode(ObjLoading::Configuration.MenuPermissiveParsing);
            return reader.ReadMenuFile();
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace menu
{
    std::unique_ptr<AssetCreator<AssetMenuList>> CreateMenuListLoaderIW3(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<MenuListLoader>(memory, searchPath);
    }
} // namespace menu
