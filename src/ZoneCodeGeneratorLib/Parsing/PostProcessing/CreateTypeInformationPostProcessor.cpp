#include "CreateTypeInformationPostProcessor.h"

bool CreateTypeInformationPostProcessor::PostProcess(IDataRepository* repository)
{
    for (auto* _enum : repository->GetAllEnums())
    {
        const auto* information = repository->GetTypeInformationFor(_enum);
        if (information == nullptr)
            repository->Add(std::make_unique<TypeInformation>(_enum));
    }

    for (auto* _typeDef : repository->GetAllTypedefs())
    {
        const auto* information = repository->GetTypeInformationFor(_typeDef);
        if (information == nullptr)
            repository->Add(std::make_unique<TypeInformation>(_typeDef));
    }

    for (auto* _struct : repository->GetAllStructs())
    {
        const auto* information = repository->GetTypeInformationFor(_struct);
        if (information == nullptr)
            repository->Add(std::make_unique<TypeInformation>(_struct));
    }

    for (auto* _union : repository->GetAllUnions())
    {
        const auto* information = repository->GetTypeInformationFor(_union);
        if (information == nullptr)
            repository->Add(std::make_unique<TypeInformation>(_union));
    }

    return true;
}
