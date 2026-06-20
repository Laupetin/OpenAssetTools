#include "FlameTableLoaderT6.h"

#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "Game/T6/Weapon/FlameTableFields.h"
#include "Weapon/WeaponCommon.h"

#include <cassert>
#include <format>

using namespace T6;

namespace
{
    class InfoStringToFlameTableConverter final : public InfoStringToStructConverter
    {
    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            assert(false);
            return false;
        }

    public:
        InfoStringToFlameTableConverter(const InfoString& infoString,
                                        FlameTable& flameTable,
                                        ZoneScriptStrings& zoneScriptStrings,
                                        MemoryManager& memory,
                                        AssetCreationContext& context,
                                        AssetRegistration<SubAssetFlameTable>& registration,
                                        const cspField_t* fields,
                                        const size_t fieldCount)
            : InfoStringToStructConverter(infoString, &flameTable, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }
    };

    class FlameTableLoaderT6 final : public SubAssetCreator<SubAssetFlameTable>
    {
    public:
        FlameTableLoaderT6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_memory(memory),
              m_search_path(searchPath),
              m_zone(zone)
        {
        }

        AssetCreationResult CreateSubAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = weapon::GetFileNameForFlameTable(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(INFO_STRING_PREFIX_FLAME_TABLE, *file.m_stream))
            {
                con::error("Could not parse as info string file: \"{}\"", fileName);
                return AssetCreationResult::Failure();
            }

            auto* flameTable = m_memory.Alloc<FlameTable>();
            AssetRegistration<SubAssetFlameTable> registration(assetName, flameTable);
            InfoStringToFlameTableConverter converter(
                infoString, *flameTable, m_zone.m_script_strings, m_memory, context, registration, flameTableFields, std::extent_v<decltype(flameTableFields)>);
            if (!converter.Convert())
            {
                con::error("Failed to parse flame table: \"{}\"", assetName);
                return AssetCreationResult::Failure();
            }

            // The flametable infostring contains the name but we don't to use it from there
            // It's kept to keep compatiblity with the official modtools and game
            flameTable->name = m_memory.Dup(assetName.c_str());

            return AssetCreationResult::Success(context.AddSubAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        Zone& m_zone;
    };
} // namespace

namespace weapon
{
    std::unique_ptr<ISubAssetCreator> CreateFlameTableLoaderT6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<FlameTableLoaderT6>(memory, searchPath, zone);
    }
} // namespace weapon
