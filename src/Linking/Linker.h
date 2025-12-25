#pragma once

#include "LinkerArgs.h"

#include <memory>

class Linker
{
public:
    virtual ~Linker() = default;

    static std::unique_ptr<Linker> Create(LinkerArgs args);

    /**
     * \brief Starts the Linker application logic.
     * \param argc The amount of command line arguments specified.
     * \param argv The command line arguments.
     * \return \c true if the application was successful or \c false if an error occurred.
     */
    virtual bool Start() = 0;
};
