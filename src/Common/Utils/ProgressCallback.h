#pragma once

#include <cstdlib>

class ProgressCallback
{
public:
    virtual void OnProgress(size_t current, size_t total) = 0;
};
