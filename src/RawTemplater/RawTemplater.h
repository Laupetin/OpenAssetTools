#pragma once

class RawTemplater
{
    class Impl;
    Impl* m_impl;

public:
    RawTemplater();
    ~RawTemplater();
    RawTemplater(const RawTemplater& other) = delete;
    RawTemplater(RawTemplater&& other) noexcept = default;
    RawTemplater& operator=(const RawTemplater& other) = delete;
    RawTemplater& operator=(RawTemplater&& other) noexcept = default;

    int Run(int argc, const char** argv) const;
};
