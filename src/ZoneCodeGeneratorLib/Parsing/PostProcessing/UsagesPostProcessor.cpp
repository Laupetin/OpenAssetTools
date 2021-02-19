#include "UsagesPostProcessor.h"

#include <algorithm>
#include <queue>
#include <set>

#include "Domain/Computations/MemberComputations.h"
#include "Domain/Computations/StructureComputations.h"

bool UsagesPostProcessor::ProcessAsset(StructureInformation* info)
{
    std::set<StructureInformation*> processedInfos;
    std::queue<StructureInformation*> processingQueue;

    processingQueue.push(info);

    while(!processingQueue.empty())
    {
        auto* currentStructure = processingQueue.front();
        processingQueue.pop();

        if(processedInfos.find(currentStructure) != processedInfos.end())
            continue;
        processedInfos.emplace(currentStructure);

        for (const auto& member : currentStructure->m_ordered_members)
        {
            if(member->m_type == nullptr)
                continue;

            const MemberComputations computations(member.get());

            if(computations.ShouldIgnore())
                continue;

            if (computations.ContainsNonEmbeddedReference())
                member->m_type->m_non_embedded_reference_exists = true;

            if (computations.ContainsSinglePointerReference())
                member->m_type->m_single_pointer_reference_exists = true;

            if (computations.ContainsArrayPointerReference())
                member->m_type->m_array_pointer_reference_exists = true;

            if (computations.ContainsArrayReference())
                member->m_type->m_array_reference_exists = true;

            if (computations.IsNotInDefaultNormalBlock())
                member->m_type->m_reference_from_non_default_normal_block_exists = true;

            member->m_type->m_usages.push_back(currentStructure);
            processingQueue.push(member->m_type);
        }
    }

    return true;
}

bool UsagesPostProcessor::PostProcess(IDataRepository* repository)
{
    const auto& allInfos = repository->GetAllStructureInformation();

    return std::all_of(allInfos.begin(), allInfos.end(), [](StructureInformation* info)
    {
        const StructureComputations computations(info);
        return !computations.IsAsset() || ProcessAsset(info);
    });
}
