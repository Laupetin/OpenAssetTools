#pragma once
#include "SearchPath/ISearchPath.h"

#include <filesystem>
#include <memory>
#include <ostream>

class SoundBankWriter
{
public:
    SoundBankWriter() = default;
    virtual ~SoundBankWriter() = default;

    SoundBankWriter(const SoundBankWriter& other) = default;
    SoundBankWriter(SoundBankWriter&& other) noexcept = default;
    SoundBankWriter& operator=(const SoundBankWriter& other) = default;
    SoundBankWriter& operator=(SoundBankWriter&& other) noexcept = default;

    virtual void AddSound(const std::string& soundFilePath, unsigned int soundId, bool looping = false, bool streamed = false) = 0;
    virtual std::int64_t Write() = 0;

    static std::unique_ptr<SoundBankWriter> Create(const std::string& fileName, std::ostream& stream, ISearchPath* assetSearchPath);

    static std::filesystem::path OutputPath;
};
