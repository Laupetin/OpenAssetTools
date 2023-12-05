#pragma once

#include "Csv/CsvStream.h"
#include "Dumping/AssetDumpingContext.h"
#include "Game/T6/T6.h"
#include "ObjContainer/SoundBank/SoundBankTypes.h"

namespace T6::sound
{
    class AliasDumperCsv
    {
    public:
        explicit AliasDumperCsv(const AssetDumpingContext& context);

        void DumpSndBankAliases(const SndBank* sndBank) const;

    private:
        std::unique_ptr<std::ostream> OpenAliasOutputFile(const SndBank* sndBank) const;
        static void WriteAliasFileHeader(CsvOutputStream& stream);
        static bool FindSoundBankEntry(unsigned assetId, SoundAssetBankEntry& entry);
        void WriteAliasToFile(CsvOutputStream& stream, const SndAlias* alias) const;
        static void WriteColumnNullSafe(CsvOutputStream& stream, const char* value);
        static void WriteColumnFloat8(CsvOutputStream& stream, uint8_t value);
        static void WriteColumnFloat16(CsvOutputStream& stream, uint16_t value);
        static void WriteColumnSignedNumeric(CsvOutputStream& stream, int value);
        static void WriteColumnUnsignedNumeric(CsvOutputStream& stream, unsigned value);

        const AssetDumpingContext& m_context;
    };
}