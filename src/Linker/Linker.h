#pragma once

#include <memory>

class Linker
{
public:
    Linker() = default;
    virtual ~Linker() = default;

    Linker(const Linker& other) = default;
    Linker(Linker&& other) noexcept = default;
    Linker& operator=(const Linker& other) = default;
    Linker& operator=(Linker&& other) noexcept = default;

    static std::unique_ptr<Linker> Create();

    /**
     * \brief Starts the Linker application logic.
     * \param argc The amount of command line arguments specified.
     * \param argv The command line arguments.
     * \return \c true if the application was successful or \c false if an error occurred.
     */
    virtual bool Start(int argc, const char** argv) = 0;
};
