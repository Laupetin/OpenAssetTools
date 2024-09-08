#include "AssetLoaderFontIcon.h"

#include "Csv/CsvStream.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/T6.h"
#include "Pool/GlobalAssetPool.h"

#include <algorithm>
#include <cstring>
#include <sstream>

using namespace T6;

void* AssetLoaderFontIcon::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetFontIcon::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}

bool AssetLoaderFontIcon::CanLoadFromRaw() const
{
    return true;
}

std::string AssetLoaderFontIcon::ErrorPrefix(const std::string& assetName, const unsigned rowIndex)
{
    std::ostringstream str;
    str << "FontIcon \"" << assetName << "\" Row " << rowIndex << ": ";
    return str.str();
}

void AssetLoaderFontIcon::PreprocessRow(std::vector<std::string>& row)
{
    for (auto& cell : row)
    {
        for (auto c : cell)
        {
            if (isspace(c))
                continue;
            if (c == '#')
                cell = "";
            break;
        }
    }
}

bool AssetLoaderFontIcon::RowIsEmpty(const std::vector<std::string>& row)
{
    return std::ranges::all_of(row,
                               [](const std::string& cell)
                               {
                                   return cell.empty();
                               });
}

bool AssetLoaderFontIcon::ParseInt(int& value, const std::string& str)
{
    char* endPtr;
    value = strtol(str.c_str(), &endPtr, 0);
    if (endPtr != &str[str.size()])
        return false;
    return true;
}

bool AssetLoaderFontIcon::ParseFloat(float& value, const std::string& str)
{
    char* endPtr;
    value = strtof(str.c_str(), &endPtr);
    if (endPtr != &str[str.size()])
        return false;
    return true;
}

bool AssetLoaderFontIcon::ParseHashStr(int& value, const std::string& str)
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

bool AssetLoaderFontIcon::ReadIconRow(const std::vector<std::string>& row,
                                      FontIconEntry& icon,
                                      const std::string& assetName,
                                      const unsigned rowIndex,
                                      MemoryManager* memory,
                                      IAssetLoadingManager* manager,
                                      std::vector<XAssetInfoGeneric*>& dependencies)
{
    if (row.size() < COL_COUNT_ICON)
    {
        std::cout << ErrorPrefix(assetName, rowIndex) << "Column count lower than min column count for entries (" << COL_COUNT_ICON << ")\n";
        return false;
    }

    if (!ParseInt(icon.fontIconSize, row[ROW_ICON_SIZE]))
    {
        std::cout << ErrorPrefix(assetName, rowIndex) << "Failed to parse size\n";
        return false;
    }

    if (!ParseFloat(icon.xScale, row[ROW_ICON_XSCALE]) || !ParseFloat(icon.yScale, row[ROW_ICON_YSCALE]))
    {
        std::cout << ErrorPrefix(assetName, rowIndex) << "Failed to parse scale\n";
        return false;
    }

    auto* materialDependency = manager->LoadDependency<AssetMaterial>(row[ROW_ICON_MATERIAL]);
    if (materialDependency == nullptr)
    {
        std::cout << ErrorPrefix(assetName, rowIndex) << "Failed to load material \"" << row[ROW_ICON_MATERIAL] << "\"\n";
        return false;
    }

    icon.fontIconMaterialHandle = materialDependency->Asset();
    icon.fontIconName.string = memory->Dup(row[ROW_ICON_NAME].c_str());
    icon.fontIconName.hash = Common::Com_HashString(icon.fontIconName.string);

    return true;
}

bool AssetLoaderFontIcon::ReadAliasRow(const std::vector<std::string>& row,
                                       FontIconAlias& alias,
                                       const std::string& assetName,
                                       const unsigned rowIndex,
                                       MemoryManager* memory,
                                       IAssetLoadingManager* manager)
{
    if (row.size() < COL_COUNT_ALIAS)
    {
        std::cout << ErrorPrefix(assetName, rowIndex) << "Column count lower than min column count for aliases (" << COL_COUNT_ALIAS << ")\n";
        return false;
    }

    if (!ParseHashStr(alias.aliasHash, row[ROW_ALIAS_NAME]))
    {
        std::cout << ErrorPrefix(assetName, rowIndex) << "Failed to parse alias \"" << row[ROW_ALIAS_NAME] << "\"\n";
        return false;
    }

    if (!ParseHashStr(alias.buttonHash, row[ROW_ALIAS_BUTTON]))
    {
        std::cout << ErrorPrefix(assetName, rowIndex) << "Failed to parse button \"" << row[ROW_ALIAS_BUTTON] << "\"\n";
        return false;
    }

    return true;
}

bool AssetLoaderFontIcon::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(assetName);
    if (!file.IsOpen())
        return false;

    auto* fontIcon = memory->Create<FontIcon>();
    fontIcon->name = memory->Dup(assetName.c_str());

    const CsvInputStream csv(*file.m_stream);
    std::vector<XAssetInfoGeneric*> dependencies;
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
            std::cout << ErrorPrefix(assetName, currentRowIndex) << "Column count lower than min column count (" << COL_COUNT_MIN << ")\n";
            return true;
        }

        int index;
        if (!ParseInt(index, currentRow[ROW_INDEX]) || index < 0)
        {
            std::cout << ErrorPrefix(assetName, currentRowIndex) << "Failed to parse index\n";
            return true;
        }

        if (currentRow[ROW_TYPE] == VALUE_TYPE_ICON)
        {
            FontIconEntry icon{};
            if (!ReadIconRow(currentRow, icon, assetName, currentRowIndex, memory, manager, dependencies))
                return true;

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
            if (!ReadAliasRow(currentRow, alias, assetName, currentRowIndex, memory, manager))
                return true;

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
            std::cout << ErrorPrefix(assetName, currentRowIndex) << "Unknown row type \"" << currentRow[ROW_TYPE] << "\"\n";
            return true;
        }
    }

    fontIcon->numEntries = entries.size();
    fontIcon->numAliasEntries = aliases.size();

    if (fontIcon->numEntries > 0)
    {
        fontIcon->fontIconEntry = memory->Alloc<FontIconEntry>(fontIcon->numEntries);
        for (auto i = 0u; i < entries.size(); i++)
            fontIcon->fontIconEntry[i] = entries[i];
    }
    else
        fontIcon->fontIconEntry = nullptr;

    if (fontIcon->numAliasEntries > 0)
    {
        fontIcon->fontIconAlias = memory->Alloc<FontIconAlias>(fontIcon->numAliasEntries);
        for (auto i = 0u; i < aliases.size(); i++)
            fontIcon->fontIconAlias[i] = aliases[i];
    }
    else
        fontIcon->fontIconAlias = nullptr;

    manager->AddAsset<AssetFontIcon>(assetName, fontIcon);

    return true;
}
