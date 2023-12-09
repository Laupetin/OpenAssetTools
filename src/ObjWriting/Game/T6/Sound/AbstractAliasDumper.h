#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/T6/T6.h"
#include "ObjContainer/SoundBank/SoundBankTypes.h"

#include <memory>
#include <ostream>
#include <string>

namespace T6::sound
{
    class AbstractAliasDumper
    {
    protected:
        explicit AbstractAliasDumper(const AssetDumpingContext& context);

    public:
        virtual ~AbstractAliasDumper() = default;
        AbstractAliasDumper(const AbstractAliasDumper& other) = default;
        AbstractAliasDumper(AbstractAliasDumper&& other) noexcept = default;
        AbstractAliasDumper& operator=(const AbstractAliasDumper& other) = delete;
        AbstractAliasDumper& operator=(AbstractAliasDumper&& other) noexcept = delete;

        virtual void DumpSndBankAliases(const SndBank* sndBank) const = 0;

    protected:
        static bool FindSoundBankEntry(unsigned assetId, SoundAssetBankEntry& entry);
        std::unique_ptr<std::ostream> OpenAliasOutputFile(const SndBank* sndBank, const std::string& extension) const;

        const AssetDumpingContext& m_context;
    };
}