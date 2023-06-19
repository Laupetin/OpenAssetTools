#pragma once

#include <memory>

class Compiler
{
public:
    Compiler() = default;
    virtual ~Compiler() = default;

    Compiler(const Compiler& other) = default;
    Compiler(Compiler&& other) noexcept = default;
    Compiler& operator=(const Compiler& other) = default;
    Compiler& operator=(Compiler&& other) noexcept = default;

    static std::unique_ptr<Compiler> Create();

    /**
     * \brief Starts the Compiler application logic.
     * \param argc The amount of command line arguments specified.
     * \param argv The command line arguments.
     * \return \c true if the application was successful or \c false if an error occurred.
     */
    virtual bool Start(int argc, const char** argv) = 0;
};