#include "UnionsPostProcessor.h"

#include "Utils/Logging/Log.h"

#include <algorithm>
#include <format>
#include <iostream>

namespace
{
    bool ProcessUnion(StructureInformation* info)
    {
        auto index = 0u;
        auto lastEntryWithoutCondition = 0u;
        auto entriesWithoutConditionCount = 0u;

        for (const auto& member : info->m_ordered_members)
        {
            if (!member->m_condition && !member->m_is_leaf)
            {
                entriesWithoutConditionCount++;
                lastEntryWithoutCondition = index;
            }
            index++;
        }

        if (entriesWithoutConditionCount > 1 && !info->m_usages.empty() && !info->m_is_leaf)
        {
            con::error("Union '{}' has more than one entry without a condition!", info->m_definition->GetFullName());
            return false;
        }

        if (entriesWithoutConditionCount == 1)
        {
            // If there is only one entry without condition make it the last of the ordered members
            auto entryWithoutCondition = std::move(info->m_ordered_members.at(lastEntryWithoutCondition));
            info->m_ordered_members.erase(info->m_ordered_members.begin() + lastEntryWithoutCondition);
            info->m_ordered_members.emplace_back(std::move(entryWithoutCondition));
        }

        return true;
    }
} // namespace

bool UnionsPostProcessor::PostProcess(IDataRepository* repository)
{
    const auto& allInfos = repository->GetAllStructureInformation();

    return std::ranges::all_of(allInfos,
                               [](StructureInformation* info)
                               {
                                   if (info->m_definition->GetType() != DataDefinitionType::UNION)
                                       return true;

                                   return ProcessUnion(info);
                               });
}
