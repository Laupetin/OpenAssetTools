#pragma once

#include "AbstractAliasDumper.h"
#include "Csv/CsvStream.h"
#include "Dumping/AssetDumpingContext.h"
#include "Game/T6/T6.h"
#include "ObjContainer/SoundBank/SoundBankTypes.h"

namespace T6::sound
{
    class AliasDumperCsv : AbstractAliasDumper
    {
    public:
        explicit AliasDumperCsv(const AssetDumpingContext& context);

        void DumpSndBankAliases(const SndBank* sndBank) const override;

    private:
        static void WriteAliasFileHeader(CsvOutputStream& stream);
        static void WriteAliasToFile(CsvOutputStream& stream, const SndAlias* alias);
        static void WriteColumnNullSafe(CsvOutputStream& stream, const char* value);
        static void WriteColumnFloat8(CsvOutputStream& stream, uint8_t value);
        static void WriteColumnFloat16(CsvOutputStream& stream, uint16_t value);
        static void WriteColumnSignedNumeric(CsvOutputStream& stream, int value);
        static void WriteColumnUnsignedNumeric(CsvOutputStream& stream, unsigned value);
    };
}