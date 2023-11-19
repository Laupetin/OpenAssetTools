#pragma once
#include "AssetList.h"
#include "Csv/CsvStream.h"

#include <iostream>

class AssetListInputStream
{
    CsvInputStream m_stream;

public:
    explicit AssetListInputStream(std::istream& stream);

    bool NextEntry(AssetListEntry& entry) const;
};

class AssetListOutputStream
{
    CsvOutputStream m_stream;

public:
    explicit AssetListOutputStream(std::ostream& stream);

    void WriteEntry(const AssetListEntry& entry);
};
