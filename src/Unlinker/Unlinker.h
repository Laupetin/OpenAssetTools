#pragma once

#include <memory>

class Unlinker
{
public:
    Unlinker() = default;
    virtual ~Unlinker() = default;

    Unlinker(const Unlinker& other) = default;
    Unlinker(Unlinker&& other) noexcept = default;
    Unlinker& operator=(const Unlinker& other) = default;
    Unlinker& operator=(Unlinker&& other) noexcept = default;

    static std::unique_ptr<Unlinker> Create();

    /**
     * \brief Starts the Unlinker application logic.
     * \param argc The amount of command line arguments specified.
     * \param argv The command line arguments.
     * \return \c true if the application was successful or \c false if an error occurred.
     */
    virtual bool Start(int argc, const char** argv) = 0;
};
