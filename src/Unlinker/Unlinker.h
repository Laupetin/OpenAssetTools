#pragma once

class Unlinker
{
    class Impl;
    Impl* m_impl;

public:
    Unlinker();
    ~Unlinker();

    Unlinker(const Unlinker& other) = delete;
    Unlinker(Unlinker&& other) noexcept = delete;
    Unlinker& operator=(const Unlinker& other) = delete;
    Unlinker& operator=(Unlinker&& other) noexcept = delete;

    /**
     * \brief Starts the Unlinker application logic.
     * \param argc The amount of command line arguments specified.
     * \param argv The command line arguments.
     * \return \c true if the application was successful or \c false if an error occurred.
     */
    bool Start(int argc, const char** argv) const;
};
