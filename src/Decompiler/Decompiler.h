#pragma once

#include <memory>

class Decompiler
{
public:
    Decompiler() = default;
    virtual ~Decompiler() = default;

    Decompiler(const Decompiler& other) = default;
    Decompiler(Decompiler&& other) noexcept = default;
    Decompiler& operator=(const Decompiler& other) = default;
    Decompiler& operator=(Decompiler&& other) noexcept = default;

    static std::unique_ptr<Decompiler> Create();

    /**
     * \brief Starts the Decompiler application logic.
     * \param argc The amount of command line arguments specified.
     * \param argv The command line arguments.
     * \return \c true if the application was successful or \c false if an error occurred.
     */
    virtual bool Start(int argc, const char** argv) = 0;
};