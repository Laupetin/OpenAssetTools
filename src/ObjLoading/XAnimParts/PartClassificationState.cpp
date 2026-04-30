#include "PartClassificationState.h"

#include "Csv/CsvStream.h"
#include "ObjLoading.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <format>
#include <iostream>

PartClassificationState::PartClassificationState()
    : m_loaded(false)
{
}

bool PartClassificationState::Load(const char** hitLocNames, const size_t hitLocNameCount, ISearchPath& searchPath)
{
    if (m_loaded)
        return true;

    con::debug("Loading part classification...");

    const auto file = searchPath.Open(PART_CLASSIFICATION_FILE);
    if (!file.IsOpen())
    {
        con::error("Could not load part classification: Failed to open {}", PART_CLASSIFICATION_FILE);
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
        con::error("Could not load part classification: Invalid row");
        return false;
    }

    utils::MakeStringLowerCase(row[0]);
    utils::MakeStringLowerCase(row[1]);

    const auto foundHitLoc = std::find(hitLocStart, hitLocEnd, row[1]);
    if (foundHitLoc == hitLocEnd)
    {
        con::error("Invalid hitloc name in row {}: {}", rowIndex + 1, row[1]);
        return false;
    }

    const auto hitLocNum = static_cast<unsigned>(std::distance(hitLocStart, foundHitLoc));

    m_part_classifications.emplace(row[0], hitLocNum);
    return true;
}
