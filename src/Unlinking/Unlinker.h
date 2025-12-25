#pragma once

#include "UnlinkerArgs.h"

#include <memory>

class Unlinker
{
public:
    virtual ~Unlinker() = default;

    static std::unique_ptr<Unlinker> Create(UnlinkerArgs args);

    /**
     * \brief Starts the Unlinker application logic.
     * \return \c true if the application was successful or \c false if an error occurred.
     */
    virtual bool Start() = 0;
};
