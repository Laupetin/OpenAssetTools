#pragma once

class Linker
{
    class Impl;
    Impl* m_impl;

public:
    Linker();
    ~Linker();

    Linker(const Linker& other) = delete;
    Linker(Linker&& other) noexcept = delete;
    Linker& operator=(const Linker& other) = delete;
    Linker& operator=(Linker&& other) noexcept = delete;

    /**
     * \brief Starts the Linker application logic.
     * \param argc The amount of command line arguments specified.
     * \param argv The command line arguments.
     * \return \c true if the application was successful or \c false if an error occurred.
     */
    bool Start(int argc, const char** argv) const;
};