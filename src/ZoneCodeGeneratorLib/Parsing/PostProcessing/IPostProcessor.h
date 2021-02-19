#pragma once
#include "Persistence/IDataRepository.h"

class IPostProcessor
{
public:
    IPostProcessor() = default;
    virtual ~IPostProcessor() = default;
    IPostProcessor(const IPostProcessor& other) = default;
    IPostProcessor(IPostProcessor&& other) noexcept = default;
    IPostProcessor& operator=(const IPostProcessor& other) = default;
    IPostProcessor& operator=(IPostProcessor&& other) noexcept = default;

    virtual bool PostProcess(IDataRepository* repository) = 0;
};
