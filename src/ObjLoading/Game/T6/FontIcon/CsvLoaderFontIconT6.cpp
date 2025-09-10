#include "CsvLoaderFontIconT6.h"

#include "Csv/CsvStream.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/T6.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <format>

using namespace T6;

namespace
{
    constexpr unsigned ROW_INDEX = 0;
    constexpr unsigned ROW_TYPE = 1;

    constexpr unsigned ROW_ICON_NAME = 2;
    constexpr unsigned ROW_ICON_MATERIAL = 3;
    constexpr unsigned ROW_ICON_SIZE = 4;
    constexpr unsigned ROW_ICON_XSCALE = 5;
    constexpr unsigned ROW_ICON_YSCALE = 6;

    constexpr unsigned ROW_ALIAS_NAME = 2;
    constexpr unsigned ROW_ALIAS_BUTTON = 3;

    constexpr auto VALUE_TYPE_ICON = "icon";
    constexpr auto VALUE_TYPE_ALIAS = "alias";

    constexpr unsigned COL_COUNT_ICON = 7;
    constexpr unsigned COL_COUNT_ALIAS = 4;
    constexpr unsigned COL_COUNT_MIN = std::min(COL_COUNT_ICON, COL_COUNT_ALIAS);

    class CsvFontIconLoader final : public AssetCreator<AssetFontIcon>
    {
    public:
        CsvFontIconLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(assetName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* fontIcon = m_memory.Alloc<FontIcon>();
            fontIcon->name = m_memory.Dup(assetName.c_str());
            AssetRegistration<AssetFontIcon> registration(assetName, fontIcon);

            const CsvInputStream csv(*file.m_stream);
            std::vector<std::string> currentRow;
            std::vector<FontIconEntry> entries;
            std::vector<FontIconAlias> aliases;
            auto currentRowIndex = 0u;

            while (csv.NextRow(currentRow))
            {
                currentRowIndex++;
                PreprocessRow(currentRow);

                if (RowIsEmpty(currentRow))
                    continue;

                if (currentRow.size() < COL_COUNT_MIN)
                {
                    con::error("{} Column count lower than min column count ({})", ErrorPrefix(assetName, currentRowIndex), COL_COUNT_MIN);
                    return AssetCreationResult::Failure();
                }

                int index;
                if (!ParseInt(index, currentRow[ROW_INDEX]) || index < 0)
                {
                    con::error("{} Failed to parse index", ErrorPrefix(assetName, currentRowIndex));
                    return AssetCreationResult::Failure();
                }

                if (currentRow[ROW_TYPE] == VALUE_TYPE_ICON)
                {
                    FontIconEntry icon{};
                    if (!ReadIconRow(currentRow, icon, assetName, currentRowIndex, context, registration))
                        return AssetCreationResult::Failure();

                    if (static_cast<unsigned>(index) == entries.size())
                    {
                        entries.push_back(icon);
                    }
                    else if (static_cast<unsigned>(index) > entries.size())
                    {
                        entries.reserve(index + 1);
                        entries[index] = icon;
                    }
                    else
                    {
                        entries[index] = icon;
                    }
                }
                else if (currentRow[ROW_TYPE] == VALUE_TYPE_ALIAS)
                {
                    FontIconAlias alias{};
                    if (!ReadAliasRow(currentRow, alias, assetName, currentRowIndex, context))
                        return AssetCreationResult::Failure();

                    if (static_cast<unsigned>(index) == aliases.size())
                    {
                        aliases.push_back(alias);
                    }
                    else if (static_cast<unsigned>(index) > aliases.size())
                    {
                        aliases.reserve(index + 1);
                        aliases[index] = alias;
                    }
                    else
                    {
                        aliases[index] = alias;
                    }
                }
                else
                {
                    con::error("{} Unknown row type \"{}\"", ErrorPrefix(assetName, currentRowIndex), currentRow[ROW_TYPE]);
                    return AssetCreationResult::Failure();
                }
            }

            fontIcon->numEntries = static_cast<unsigned>(entries.size());
            fontIcon->numAliasEntries = static_cast<unsigned>(aliases.size());

            if (fontIcon->numEntries > 0)
            {
                fontIcon->fontIconEntry = m_memory.Alloc<FontIconEntry>(fontIcon->numEntries);
                for (auto i = 0u; i < entries.size(); i++)
                    fontIcon->fontIconEntry[i] = entries[i];
            }
            else
                fontIcon->fontIconEntry = nullptr;

            if (fontIcon->numAliasEntries > 0)
            {
                fontIcon->fontIconAlias = m_memory.Alloc<FontIconAlias>(fontIcon->numAliasEntries);
                for (auto i = 0u; i < aliases.size(); i++)
                    fontIcon->fontIconAlias[i] = aliases[i];
            }
            else
                fontIcon->fontIconAlias = nullptr;

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        static std::string ErrorPrefix(const std::string& assetName, const unsigned rowIndex)
        {
            return std::format("FontIcon \"{}\" Row {}:", assetName, rowIndex);
        }

        static void PreprocessRow(std::vector<std::string>& row)
        {
            for (auto& cell : row)
            {
                for (const auto c : cell)
                {
                    if (isspace(c))
                        continue;
                    if (c == '#')
                        cell = "";
                    break;
                }
            }
        }

        static bool RowIsEmpty(const std::vector<std::string>& row)
        {
            return std::ranges::all_of(row,
                                       [](const std::string& cell)
                                       {
                                           return cell.empty();
                                       });
        }

        static bool ParseInt(int& value, const std::string& str)
        {
            char* endPtr;
            value = strtol(str.c_str(), &endPtr, 0);
            if (endPtr != &str[str.size()])
                return false;
            return true;
        }

        static bool ParseFloat(float& value, const std::string& str)
        {
            char* endPtr;
            value = strtof(str.c_str(), &endPtr);
            if (endPtr != &str[str.size()])
                return false;
            return true;
        }

        static bool ParseHashStr(int& value, const std::string& str)
        {
            if (!str.empty() && str[0] == '@' && str.size() > 1)
            {
                char* endPtr;
                value = strtol(&str[1], &endPtr, 16);

                if (endPtr != &str[str.size()])
                    return false;
            }
            else
            {
                value = Common::Com_HashString(str.c_str());
            }

            return true;
        }

        bool ReadIconRow(const std::vector<std::string>& row,
                         FontIconEntry& icon,
                         const std::string& assetName,
                         const unsigned rowIndex,
                         AssetCreationContext& context,
                         AssetRegistration<AssetFontIcon>& registration) const
        {
            if (row.size() < COL_COUNT_ICON)
            {
                con::error("{} Column count lower than min column count for entries ({})", ErrorPrefix(assetName, rowIndex), COL_COUNT_ICON);
                return false;
            }

            if (!ParseInt(icon.fontIconSize, row[ROW_ICON_SIZE]))
            {
                con::error("{} Failed to parse size", ErrorPrefix(assetName, rowIndex));
                return false;
            }

            if (!ParseFloat(icon.xScale, row[ROW_ICON_XSCALE]) || !ParseFloat(icon.yScale, row[ROW_ICON_YSCALE]))
            {
                con::error("{} Failed to parse scale", ErrorPrefix(assetName, rowIndex));
                return false;
            }

            auto* materialDependency = context.LoadDependency<AssetMaterial>(row[ROW_ICON_MATERIAL]);
            if (materialDependency == nullptr)
            {
                con::error("{} Failed to load material \"{}\"", ErrorPrefix(assetName, rowIndex), row[ROW_ICON_MATERIAL]);
                return false;
            }
            registration.AddDependency(materialDependency);

            icon.fontIconMaterialHandle = materialDependency->Asset();
            icon.fontIconName.string = m_memory.Dup(row[ROW_ICON_NAME].c_str());
            icon.fontIconName.hash = Common::Com_HashString(icon.fontIconName.string);

            return true;
        }

        static bool ReadAliasRow(
            const std::vector<std::string>& row, FontIconAlias& alias, const std::string& assetName, const unsigned rowIndex, AssetCreationContext& context)
        {
            if (row.size() < COL_COUNT_ALIAS)
            {
                con::error("{} Column count lower than min column count for aliases ({})", ErrorPrefix(assetName, rowIndex), COL_COUNT_ALIAS);
                return false;
            }

            if (!ParseHashStr(alias.aliasHash, row[ROW_ALIAS_NAME]))
            {
                con::error("{} Failed to parse alias \"{}\"", ErrorPrefix(assetName, rowIndex), row[ROW_ALIAS_NAME]);
                return false;
            }

            if (!ParseHashStr(alias.buttonHash, row[ROW_ALIAS_BUTTON]))
            {
                con::error("{} Failed to parse button \"{}\"", ErrorPrefix(assetName, rowIndex), row[ROW_ALIAS_BUTTON]);
                return false;
            }

            return true;
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace font_icon
{
    std::unique_ptr<AssetCreator<AssetFontIcon>> CreateCsvLoaderT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<CsvFontIconLoader>(memory, searchPath);
    }
} // namespace font_icon
