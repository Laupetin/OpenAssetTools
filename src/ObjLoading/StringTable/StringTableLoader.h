#pragma once

#include "Csv/CsvStream.h"

#include <algorithm>
#include <istream>
#include <type_traits>
#include <vector>

namespace string_table
{
    template<typename StringTableType, typename CellType> class AbstractStringTableLoader
    {
    protected:
        AbstractStringTableLoader() = default;
        virtual ~AbstractStringTableLoader() = default;
        AbstractStringTableLoader(const AbstractStringTableLoader& other) = default;
        AbstractStringTableLoader(AbstractStringTableLoader&& other) noexcept = default;
        AbstractStringTableLoader& operator=(const AbstractStringTableLoader& other) = default;
        AbstractStringTableLoader& operator=(AbstractStringTableLoader&& other) noexcept = default;

        virtual void SetCellContent(CellType& cell, const char* content) = 0;

        virtual void PostProcessStringTable(StringTableType* stringTable, const unsigned cellCount, MemoryManager& memory) {}

    public:
        StringTableType* LoadFromStream(const std::string& assetName, MemoryManager& memory, std::istream& stream)
        {
            auto* stringTable = memory.Create<StringTableType>();
            stringTable->name = memory.Dup(assetName.c_str());

            std::vector<std::vector<std::string>> csvLines;
            std::vector<std::string> currentLine;
            auto maxCols = 0u;
            const CsvInputStream csv(stream);

            while (csv.NextRow(currentLine))
            {
                if (currentLine.size() > maxCols)
                    maxCols = currentLine.size();
                csvLines.emplace_back(std::move(currentLine));
                currentLine = std::vector<std::string>();
            }

            stringTable->columnCount = static_cast<int>(maxCols);
            stringTable->rowCount = static_cast<int>(csvLines.size());
            const auto cellCount = static_cast<unsigned>(stringTable->rowCount) * static_cast<unsigned>(stringTable->columnCount);

            if (cellCount)
            {
                stringTable->values = memory.Alloc<CellType>(cellCount);

                for (auto row = 0u; row < csvLines.size(); row++)
                {
                    const auto& rowValues = csvLines[row];
                    for (auto col = 0u; col < maxCols; col++)
                    {
                        auto& cell = stringTable->values[row * maxCols + col];
                        if (col >= rowValues.size() || rowValues[col].empty())
                            SetCellContent(cell, "");
                        else
                            SetCellContent(cell, memory.Dup(rowValues[col].c_str()));
                    }
                }
            }
            else
            {
                stringTable->values = nullptr;
            }

            PostProcessStringTable(stringTable, cellCount, memory);

            return stringTable;
        }
    };

    // =================================
    // V1: IW3
    // - Cells are const char*
    // =================================
    template<typename StringTableType> class StringTableLoaderV1 final : public AbstractStringTableLoader<StringTableType, const char*>
    {
    protected:
        void SetCellContent(const char*& cell, const char* content) override
        {
            cell = content;
        }
    };

    // =================================
    // V2: IW4, IW5
    // - Cells are a struct and have a hash
    // =================================
    template<typename StringTableType, int (*HashFunc)(const char*)>
    class StringTableLoaderV2 final : public AbstractStringTableLoader<StringTableType, std::remove_pointer_t<decltype(StringTableType::values)>>
    {
        using CellType_t = decltype(*StringTableType::values);

    protected:
        void SetCellContent(CellType_t& cell, const char* content) override
        {
            cell.string = content;
            cell.hash = HashFunc(content);
        }
    };

    // =================================
    // V3: T5, T6
    // - Cells are a struct and have a hash
    // - StringTable has an index array for binary search
    // =================================
    template<typename StringTableType, int (*HashFunc)(const char*)>
    class StringTableLoaderV3 final : public AbstractStringTableLoader<StringTableType, std::remove_pointer_t<decltype(StringTableType::values)>>
    {
        using CellType_t = decltype(*StringTableType::values);

    protected:
        void SetCellContent(CellType_t& cell, const char* content) override
        {
            cell.string = content;
            cell.hash = HashFunc(content);
        }

        void PostProcessStringTable(StringTableType* stringTable, const unsigned cellCount, MemoryManager& memory) override
        {
            if (!cellCount)
            {
                stringTable->cellIndex = nullptr;
                return;
            }

            stringTable->cellIndex = memory.Alloc<int16_t>(cellCount);
            for (auto i = 0u; i < cellCount; i++)
                stringTable->cellIndex[i] = i;

            std::sort(&stringTable->cellIndex[0],
                      &stringTable->cellIndex[cellCount - 1],
                      [stringTable](const int16_t a, const int16_t b)
                      {
                          auto compareResult = stringTable->values[a].hash - stringTable->values[b].hash;
                          if (compareResult == 0)
                              compareResult = (a % stringTable->columnCount) - (b % stringTable->columnCount);
                          return compareResult < 0;
                      });
        }
    };
} // namespace string_table
