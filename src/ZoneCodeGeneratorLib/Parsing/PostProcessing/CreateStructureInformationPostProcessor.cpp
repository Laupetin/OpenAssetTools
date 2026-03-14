#include "CreateStructureInformationPostProcessor.h"

bool CreateStructureInformationPostProcessor::PostProcess(IDataRepository* repository)
{
    for (auto* structDefinition : repository->GetAllStructs())
    {
        auto* information = repository->GetInformationFor(structDefinition);
        if (information == nullptr)
            repository->Add(std::make_unique<StructureInformation>(structDefinition, repository->GetTypeInformationFor(structDefinition)));
    }

    for (auto* unionDefinition : repository->GetAllUnions())
    {
        auto* information = repository->GetInformationFor(unionDefinition);
        if (information == nullptr)
            repository->Add(std::make_unique<StructureInformation>(unionDefinition, repository->GetTypeInformationFor(unionDefinition)));
    }

    return true;
}
