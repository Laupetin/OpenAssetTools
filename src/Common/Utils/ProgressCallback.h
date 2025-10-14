#pragma once

#include <cstdlib>

class ProgressCallback
{
public:
    ProgressCallback() = default;
    virtual ~ProgressCallback() = default;

    virtual void OnProgress(size_t current, size_t total) = 0;
};
