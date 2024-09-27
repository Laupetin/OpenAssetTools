#include "PartClassificationState.h"

#include "Csv/CsvStream.h"
#include "ObjLoading.h"
#include "Utils/StringUtils.h"

#include <format>
#include <iostream>

PartClassificationState::PartClassificationState()
    : m_loaded(false)
{
}

bool PartClassificationState::Load(const char** hitLocNames, const size_t hitLocNameCount, const IAssetLoadingManager& manager)
{
    if (m_loaded)
        return true;

    if (ObjLoading::Configuration.Verbose)
        std::cout << "Loading part classification...\n";

    const auto file = manager.GetAssetLoadingContext()->m_raw_search_path.Open(PART_CLASSIFICATION_FILE);
    if (!file.IsOpen())
    {
        std::cerr << std::format("Could not load part classification: Failed to open {}\n", PART_CLASSIFICATION_FILE);
        return false;
    }

    const auto hitLocStart = hitLocNames;
    const auto hitLocEnd = &hitLocNames[hitLocNameCount];

    const CsvInputStream csvStream(*file.m_stream);
    std::vector<std::string> row;
    auto rowIndex = 0u;
    while (csvStream.NextRow(row))
    {
        if (!LoadRow(hitLocStart, hitLocEnd, rowIndex++, row))
            return false;
    }

    m_loaded = true;

    return false;
}

[[nodiscard]] unsigned PartClassificationState::GetPartClassificationForBoneName(const std::string& boneName) const
{
    const auto entry = m_part_classifications.find(boneName);

    return entry != m_part_classifications.end() ? entry->second : HITLOC_NONE;
}

bool PartClassificationState::LoadRow(const char** hitLocStart, const char** hitLocEnd, const unsigned rowIndex, std::vector<std::string>& row)
{
    if (row.empty())
        return true;

    if (row.size() != 2)
    {
        std::cerr << "Could not load part classification: Invalid row\n";
        return false;
    }

    utils::MakeStringLowerCase(row[0]);
    utils::MakeStringLowerCase(row[1]);

    const auto foundHitLoc = std::find(hitLocStart, hitLocEnd, row[1]);
    if (foundHitLoc == hitLocEnd)
    {
        std::cerr << std::format("Invalid hitloc name in row {}: {}\n", rowIndex + 1, row[1]);
        return false;
    }

    const auto hitLocNum = std::distance(hitLocStart, foundHitLoc);

    m_part_classifications.emplace(row[0], hitLocNum);
    return true;
}
